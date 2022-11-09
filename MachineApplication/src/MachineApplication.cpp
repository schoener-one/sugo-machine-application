///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2019-12-01
 */
///////////////////////////////////////////////////////////////////////////////

#include "MachineApplication.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include "CommandLineParser.hpp"
#include "CommandMessageBroker.hpp"
#include "ConfigurationFileParser.hpp"
#include "HardwareAbstractionLayer.hpp"
#include "MachineServiceGateway.hpp"
#include "RemoteControlServer.hpp"
#include "ServiceComponentExecutionBundle.hpp"
#include "ServiceLocator.hpp"
#include "UserInterfaceControl.hpp"

namespace po = boost::program_options;
using namespace sugo;
using namespace sugo::message;
using namespace hal;

namespace
{
using MachineServiceGatewayExecutionBundle =
    ServiceComponentExecutionBundle<MachineServiceGateway, IOContext, const IConfiguration>;
}  // namespace

MachineApplication::MachineApplication(const std::string& appName)
    : m_name(appName),
      m_configCommandLine({Option("config-file", std::string(), "Configuration file")})
{
}

bool MachineApplication::parseCommandLine(int argc, char const** argv)
{
    CommandLineParser parser(argc, argv);
    parser.add(m_configCommandLine);
    return parser.parse();
}

bool MachineApplication::parseConfigurationFile()
{
    const std::string fileName = m_configCommandLine["config-file"].get<std::string>();

    if (!fileName.empty())
    {
        std::ifstream           inStream(fileName);
        ConfigurationFileParser parser(inStream);
        parser.add(m_configuration);

        return parser.parse();
    }
    else
    {
        LOG(warning) << "no configuration file passed";
        return true;
    }
}

void MachineApplication::addConfigurationOptions(IConfiguration& configuration)
{
    configuration.add(Option("machine-remote-control-service.address", std::string(""),
                             "Network address of the service"));
    configuration.add(Option("machine-remote-control-service.port",
                             static_cast<unsigned short>(8080u), "Network port of the service"));
    configuration.add(Option("machine-remote-control-service.doc-root", std::string("www"),
                             "Network address of the service"));
}

bool MachineApplication::start(int argc, char const** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    addConfigurationOptions(m_configuration);
    HardwareAbstractionLayer::addConfigurationOptions(m_configuration);
    MachineServiceGateway::addConfigurationOptions(m_configuration);

    LOG(info) << "Starting application " << m_name;
    if (!parseCommandLine(argc, argv))
    {
        return false;
    }

    if (!parseConfigurationFile())
    {
        return false;
    }

    ServiceLocator serviceLocator;
    serviceLocator.add<IConfiguration>(m_configuration);

    HardwareAbstractionLayer hal;
    serviceLocator.add<IHardwareAbstractionLayer>(hal);
    if (!serviceLocator.get<IHardwareAbstractionLayer>().init(m_configuration))
    {
        return false;
    }

    // Start service components
    if (!m_components.start(serviceLocator))
    {
        LOG(error) << "Failed to start application components";
        return false;
    }

    // Start gateway
    IOContext                            contextRpcServer("MachineServiceGatewayRpcServer");
    MachineServiceGatewayExecutionBundle machineServiceGateway(contextRpcServer, m_configuration);

    if (!machineServiceGateway.start())
    {
        LOG(error) << "Failed to start machine gateway";
        m_components.stop();
        return false;
    }

    // Start remote control server
    auto it = std::find_if(
        m_components.getBundles().begin(), m_components.getBundles().end(),
        [](const auto& bundle) { return (bundle->getId() == IUserInterfaceControl::ReceiverId); });
    assert((it != m_components.getBundles().end()) && (*it));
    IServiceComponent& serviceComponent = it->get()->getServiceComponent();

    remote_control::RemoteControlServer remoteControlServer(
        m_configuration.getOption("machine-remote-control-service.address").get<std::string>(),
        m_configuration.getOption("machine-remote-control-service.port").get<unsigned short>(),
        m_configuration.getOption("machine-remote-control-service.doc-root").get<std::string>(),
        *static_cast<UserInterfaceControl*>(&serviceComponent));

    if (!remoteControlServer.start())
    {
        LOG(error) << "Failed to start remote control server";
        machineServiceGateway.stop();
        m_components.stop();
        return false;
    }

    m_components.waitUntilFinished();
    remoteControlServer.stop();
    machineServiceGateway.stop();
    LOG(info) << "Application " << m_name << " stopped";
    return true;
}
