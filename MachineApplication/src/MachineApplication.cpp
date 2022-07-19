///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.12.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include "MachineApplication.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include "CommandLineParser.hpp"
#include "CommandMessageBroker.hpp"
#include "ConfigurationFileParser.hpp"
#include "FilamentCoilControl.hpp"
#include "FilamentCoilMotor.hpp"
#include "FilamentFeederMotor.hpp"
#include "FilamentMergerControl.hpp"
#include "FilamentMergerHeater.hpp"
#include "FilamentPreHeater.hpp"
#include "FilamentTensionSensor.hpp"
#include "Globals.hpp"
#include "HardwareAbstractionLayer.hpp"
#include "MachineApplication.hpp"
#include "MachineControl.hpp"
#include "MachineServiceGateway.hpp"
#include "UserInterfaceControl.hpp"

namespace po = boost::program_options;
using namespace sugo;
using namespace hal;

MachineApplication::MachineApplication()
    : m_configCommandLine(
          {Option("config-file", std::string("SugoMachine.json"), "Configuration file"),
           Option("real-time", false, "Indicates if the threads should run in real-time mode")})
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
    std::ifstream           inStream(m_configCommandLine["config-file"].get<std::string>());
    ConfigurationFileParser parser(inStream);
    parser.add(m_configuration);

    return parser.parse();
}

bool MachineApplication::start(int argc, char const** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // MachineServiceGateway::setConfiguration(m_configuration);
    // CupRotationTray::setConfiguration(m_configuration);
    HardwareAbstractionLayer::setConfiguration(m_configuration);

    LOG(info) << "Starting SugoMachine application";
    if (!parseCommandLine(argc, argv))
    {
        return false;
    }

    if (!parseConfigurationFile())
    {
        return false;
    }

    // HardwareAbstractionLayer hal;
    // Configuration            halConfiguration;
    // hal.getHardwareConfiguration(m_configuration, halConfiguration);
    // if (!hal.init(halConfiguration))
    // {
    //     return false;
    // }

    // ServiceLocator locator;
    // locator.register(hal);

    // SugoMachine
    CommandMessageBroker machineControllerMessageBroker(IMachineControl::ReceiverId,
                                                        m_ioContexts[typeid(MachineControl)]);
    MachineControl       machineControl(machineControllerMessageBroker);
    if (!machineControl.start())
    {
        return false;
    }

    // MachineServiceGateway
    CommandMessageBroker  machineServiceMessageBroker(IMachineServiceGateway::ReceiverId,
                                                     m_ioContexts[typeid(MachineServiceGateway)]);
    MachineServiceGateway machineService(machineServiceMessageBroker, m_configuration,
                                         m_ioContexts[typeid(MachineServiceGateway)]);
    if (!machineService.start())
    {
        LOG(error) << "Failed to start machine service";
        return false;
    }

    if (!run())
    {
        LOG(error) << "Failed to start running the application";
        return false;
    }

    return true;
}

bool MachineApplication::run()
{
    // FIXME move the start to
    const bool useRealTimePolicy = m_configCommandLine["real-time"].get<bool>();

    bool success = true;
    for (auto& ioContext : m_ioContexts)
    {
        if (useRealTimePolicy)
        {
            ioContext.second.setPolicy(Thread::PolicyRealTime, 0);
        }
        success = ioContext.second.start();
        if (!success)
        {
            LOG(error) << "Failed to start IO context";
            break;
        }
    }

    // FIXME In case of an error stop all threads here!
    if (success)
    {
        for (auto& ioContext : m_ioContexts)
        {
            ioContext.second.stop();
        }
    }
    return success;
}
