///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <string>

#include "Common/CommandLineParser.hpp"
#include "Common/ConfigurationFileParser.hpp"
#include "Common/ServiceLocator.hpp"
#include "HardwareAbstractionLayer/Configuration.hpp"
#include "HardwareAbstractionLayer/HardwareAbstractionLayer.hpp"
#include "MachineApplication/MachineApplication.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/ExecutionGroup.hpp"
#include "MachineServiceComponent/UserInterfaceControl.hpp"
#include "MessageBroker/MessageBroker.hpp"
#include "RemoteControl/Configuration.hpp"
#include "RemoteControl/RemoteControlServer.hpp"
#include "ServiceComponent/ExecutionBundle.hpp"
#include "ServiceGateway/Configuration.hpp"
#include "ServiceGateway/ServiceGateway.hpp"

using namespace sugo;
using namespace sugo::machine_application;

MachineApplication::MachineApplication(std::string appName)
    : m_name(std::move(appName)),
      m_configCommandLine(
          {common::Option("config-file", std::string(), "common::Configuration file")})
{
}

bool MachineApplication::parseCommandLine(int argc, char const** argv)
{
    common::CommandLineParser parser(argc, argv);
    parser.add(m_configCommandLine);
    return parser.parse();
}

bool MachineApplication::parseConfigurationFile()
{
    const auto fileName = m_configCommandLine["config-file"].get<std::string>();

    if (!fileName.empty())
    {
        std::ifstream                   inStream(fileName);
        common::ConfigurationFileParser parser(inStream);
        parser.add(m_configuration);

        return parser.parse();
    }
    else
    {
        LOG(warning) << "no configuration file passed";
        return true;
    }
}

void MachineApplication::addConfigurationOptions()
{
    remote_control::config::addConfigurationOptions(m_configuration);
    hal::config::addConfigurationOptions(m_configuration);
    service_gateway::config::addConfigurationOptions(m_configuration);
    machine_service_component::config::addConfigurationOptions(m_configuration);
}

bool MachineApplication::start(int argc, char const** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Prepare configuration parser
    addConfigurationOptions();

    LOG(info) << "Starting machine application " << m_name;

    if (!parseCommandLine(argc, argv))
    {
        return false;
    }

    if (!parseConfigurationFile())
    {
        return false;
    }

    // Init hardware abstraction layer
    hal::HardwareAbstractionLayer hal;

    if (!hal.init(m_configuration))
    {
        return false;
    }

    common::ServiceLocator serviceLocator;
    serviceLocator.add<common::IConfiguration>(m_configuration);
    serviceLocator.add<hal::IHardwareAbstractionLayer>(hal);

    // Start machine service components
    machine_service_component::ExecutionGroup machineServiceGroup(
        machine_service_component::MachineControlBundle{serviceLocator},
        machine_service_component::FilamentMergerControlBundle{serviceLocator},
        machine_service_component::FilamentFeederMotorBundle{serviceLocator},
        machine_service_component::FilamentMergerHeaterBundle{serviceLocator},
        machine_service_component::FilamentPreHeaterBundle{serviceLocator},
        machine_service_component::FilamentCoilControlBundle{serviceLocator},
        machine_service_component::FilamentCoilMotorBundle{serviceLocator},
        machine_service_component::FilamentTensionSensorBundle{serviceLocator},
        machine_service_component::UserInterfaceControlBundle{serviceLocator});

    if (!machineServiceGroup.start())
    {
        LOG(error) << "Failed to start machine service components";
        return false;
    }

    // Start service gateway
    using ServiceGatewayBundle =
        service_component::ExecutionBundle<service_gateway::ServiceGateway,
                                           const common::ServiceLocator&, common::IOContext&>;
    common::IOContext contextRpcServer("ServiceGatewayRpcServer");
    service_component::ExecutionGroup<ServiceGatewayBundle> machineServiceGateway(
        ServiceGatewayBundle{serviceLocator, contextRpcServer});

    if (!machineServiceGateway.start())
    {
        LOG(error) << "Failed to start machine service gateway";
        machineServiceGroup.stop();
        return false;
    }

    // Start remote control server
    auto& serviceComponent =
        machineServiceGroup.get<machine_service_component::UserInterfaceControlBundle>();
    remote_control::RemoteControlServer remoteControlServer(m_configuration,
                                                            serviceComponent.getComponent());

    if (!remoteControlServer.start())
    {
        LOG(error) << "Failed to start machine remote control server";
        machineServiceGateway.stop();
        machineServiceGroup.stop();
        return false;
    }

    machineServiceGroup.waitUntilFinished();  // blocks until machine stops!

    remoteControlServer.stop();
    machineServiceGateway.stop();
    hal.finalize();

    LOG(info) << "Machine application " << m_name << " stopped";
    return true;
}
