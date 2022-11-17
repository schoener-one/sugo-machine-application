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
using namespace remote_control;

namespace
{
using MachineServiceGatewayExecutionBundle =
    ServiceComponentExecutionBundle<MachineServiceGateway, IOContext, const IConfiguration>;

template <class _ServiceComponentT>
inline _ServiceComponentT& getServiceComponentFromBundles(
    sugo::ServiceComponentsExecutionGroup::ServiceComponentExecutionBundles& bundles)
{
    auto it = std::find_if(bundles.begin(), bundles.end(), [](const auto& bundle) {
        return (bundle->getId() == sugo::IUserInterfaceControl::ReceiverId);
    });
    assert((it != bundles.end()) && (*it));
    return static_cast<_ServiceComponentT&>(it->get()->getServiceComponent());
}
}  // namespace

MachineApplication::MachineApplication(std::string appName)
    : m_name(std::move(appName)),
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
    const auto fileName = m_configCommandLine["config-file"].get<std::string>();

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

bool MachineApplication::start(int argc, char const** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    RemoteControlServer::addConfigurationOptions(m_configuration);
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

    HardwareAbstractionLayer hal;
    if (!hal.init(m_configuration))
    {
        return false;
    }

    ServiceLocator serviceLocator;
    serviceLocator.add<IConfiguration>(m_configuration);
    serviceLocator.add<IHardwareAbstractionLayer>(hal);

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
    auto& serviceComponent =
        getServiceComponentFromBundles<UserInterfaceControl>(m_components.getBundles());
    remote_control::RemoteControlServer remoteControlServer(m_configuration, serviceComponent);

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
    hal.finalize();

    LOG(info) << "Application " << m_name << " stopped";
    return true;
}
