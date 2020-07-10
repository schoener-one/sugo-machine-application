///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.12.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <string>

#include "CircularMotionController.hpp"
#include "CoffeeAutomatApplication.hpp"
#include "CoffeeAutomatController.hpp"
#include "CoffeeAutomatService.hpp"
#include "CommandLineParser.hpp"
#include "CommandMessageBroker.hpp"
#include "ConfigurationFileParser.hpp"
#include "CupRotationTray.hpp"
#include "Globals.hpp"
#include "HardwareAbstractionLayer.hpp"
#include "PositionSwitch.hpp"
#include "StepperMotor.hpp"

namespace po = boost::program_options;
using namespace moco;

CoffeeAutomatApplication::CoffeeAutomatApplication()
    : m_configCommandLine(
          {Option("config-file", std::string("CoffeeMachine.json"), "Configuration file"),
           Option("real-time", false, "Indicates if the threads should run in real-time mode")})
{
}

bool CoffeeAutomatApplication::parseCommandLine(int argc, char const** argv)
{
    CommandLineParser parser(argc, argv);
    parser.add(m_configCommandLine);
    return parser.parse();
}

bool CoffeeAutomatApplication::parseConfigurationFile()
{
    std::ifstream           inStream(m_configCommandLine["config-file"].get<std::string>());
    ConfigurationFileParser parser(inStream);
    parser.add(m_configuration);

    return parser.parse();
}

bool CoffeeAutomatApplication::start(int argc, char const** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CoffeeAutomatService::setConfiguration(m_configuration);
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
            ICupRotationTray::Command::ReceiverId, ICoffeeAutomatController::Command::ReceiverId,
            ICoffeeAutomatService::Command::ReceiverId};

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
        CommandMessageBroker coffeeAutomatControllerMessageBroker(
            ICoffeeAutomatController::Command::ReceiverId, notificationReceivers,
            m_ioContexts[typeid(CoffeeAutomatController)]);
        CoffeeAutomatController coffeeAutomatController(coffeeAutomatControllerMessageBroker);
        if (success)
        {
            success = coffeeAutomatController.start();
        }

        // CoffeeAutomateService
        CommandMessageBroker coffeeAutomatServiceMessageBroker(
            ICoffeeAutomatService::Command::ReceiverId, notificationReceivers,
            m_ioContexts[typeid(CoffeeAutomatService)]);
        CoffeeAutomatService coffeeAutomatService(coffeeAutomatServiceMessageBroker,
                                                  m_configuration,
                                                  m_ioContexts[typeid(CoffeeAutomatService)]);
        if (success)
        {
            success = coffeeAutomatService.start();
        }

        if (success)
        {
            success = run();
        }
    }

    return success;
}

bool CoffeeAutomatApplication::run()
{
    Thread::Policy threadPolicy      = Thread::PolicyCurrent;
    const bool     useRealTimePolicy = m_configCommandLine["real-time"].get<bool>();
    if (useRealTimePolicy)
    {
        threadPolicy = Thread::PolicyRealTime;
    }

    bool success = true;
    for (auto& ioContext : m_ioContexts)
    {
        success = ioContext.second.start(threadPolicy);
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
            ioContext.second.waitUntilFinish();
        }
    }
    return success;
}
