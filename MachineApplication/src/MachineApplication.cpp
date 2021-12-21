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

#include "CircularMotionController.hpp"
#include "CommandLineParser.hpp"
#include "CommandMessageBroker.hpp"
#include "ConfigurationFileParser.hpp"
#include "CupRotationTray.hpp"
#include "Globals.hpp"
#include "HardwareAbstractionLayer.hpp"
#include "MachineController.hpp"
#include "MachineService.hpp"
#include "PositionSwitch.hpp"
#include "StepperMotor.hpp"

namespace po = boost::program_options;
using namespace moco;

MachineApplication::MachineApplication()
    : m_configCommandLine(
          {Option("config-file", std::string("CoffeeMachine.json"), "Configuration file"),
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

    MachineService::setConfiguration(m_configuration);
    CupRotationTray::setConfiguration(m_configuration);
    HardwareAbstractionLayer::setConfiguration(m_configuration);

    LOG(info) << "Starting CoffeeAutomat application";
    bool success = parseCommandLine(argc, argv);

    if (success)
    {
        success = parseConfigurationFile();
    }

    if (success)
    {
        HardwareAbstractionLayer hardwareAbstractionLayer(m_configuration);
        IHardwareAbstractionLayer::init(&hardwareAbstractionLayer);

        const CommandMessageBroker::ReceiverIdList notificationReceivers = {
            ICupRotationTray::Command::ReceiverId, IMachineController::Command::ReceiverId,
            IMachineService::Command::ReceiverId};

        // CupRotationTray
        Configuration configuration;
        StepperMotor  stepperMotor(
            CupRotationTray::getStepperMotorConfiguration(m_configuration, configuration));
        CircularMotionController::PositionSwitchArray positionSwitcheArray = {
            std::make_unique<PositionSwitch>(CupRotationTray::getGpioPin(1, m_configuration)),
            std::make_unique<PositionSwitch>(CupRotationTray::getGpioPin(1, m_configuration)),
            std::make_unique<PositionSwitch>(CupRotationTray::getGpioPin(1, m_configuration))};
        CircularMotionController circularMotionController(stepperMotor, positionSwitcheArray);
        CommandMessageBroker     cupRotationTrayMessageBroker(ICupRotationTray::Command::ReceiverId,
                                                          notificationReceivers,
                                                          m_ioContexts[typeid(CupRotationTray)]);
        CupRotationTray cupRotationTray(cupRotationTrayMessageBroker, circularMotionController);
        success = cupRotationTray.start();

        // CoffeeAutomat
        CommandMessageBroker machineControllerMessageBroker(
            IMachineController::Command::ReceiverId, notificationReceivers,
            m_ioContexts[typeid(MachineController)]);
        MachineController machineController(machineControllerMessageBroker);
        if (success)
        {
            success = machineController.start();
        }

        // CoffeeAutomateService
        CommandMessageBroker machineServiceMessageBroker(IMachineService::Command::ReceiverId,
                                                         notificationReceivers,
                                                         m_ioContexts[typeid(MachineService)]);
        MachineService       machineService(machineServiceMessageBroker, m_configuration,
                                      m_ioContexts[typeid(MachineService)]);
        if (success)
        {
            success = machineService.start();
        }

        if (success)
        {
            success = run();
        }
    }

    return success;
}

bool MachineApplication::run()
{
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
