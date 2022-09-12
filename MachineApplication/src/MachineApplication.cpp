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
#include "ServiceComponentExecutionBundle.hpp"
#include "ServiceLocator.hpp"

namespace po = boost::program_options;
using namespace sugo;
using namespace hal;

namespace
{
template <class GatewayT>
struct GatewayExecutionBundle
{
    GatewayExecutionBundle(const IConfiguration& configuration)
        : context(),
          broker(GatewayT::ReceiverId, context),
          component(broker, configuration, context)
    {
    }

    AsioContext           context;
    CommandMessageBroker  broker;
    MachineServiceGateway component;

    AsioContext& getContext()
    {
        return context;
    }
    ServiceComponent& getServiceComponent()
    {
        return component;
    }
};
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

bool MachineApplication::start(int argc, char const** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

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

    // Start gateway
    GatewayExecutionBundle<MachineServiceGateway> machineServiceGateway(m_configuration);
    if (!machineServiceGateway.getServiceComponent().start())
    {
        LOG(error) << "Failed to start machine gateway";
        return false;
    }

    // Start service components
    if (!m_components.start(serviceLocator))
    {
        LOG(error) << "Failed to start application components";
        return false;
    }

    m_components.waitUntilFinished();
    LOG(info) << "Application " << m_name << " stopped";
    return true;
}
