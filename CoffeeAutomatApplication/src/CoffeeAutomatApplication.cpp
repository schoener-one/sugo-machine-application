/*
 * CoffeeMachineApplication.cpp
 *
 *  Created on: 01.12.2019
 *      Author: denis
 */

#include <fstream>
#include <iostream>
#include <string>

#include "CoffeeAutomatApplication.hpp"

#include "CircularMotionController.hpp"
#include "CoffeeAutomat.hpp"
#include "CoffeeAutomatService.hpp"
#include "CommandLineParser.hpp"
#include "ConfigurationFileParser.hpp"
#include "CupRotationTray.hpp"
#include "Globals.hpp"
#include "HardwareAbstractionLayer.hpp"
#include "ICommandMessageBroker.hpp"
#include "PositionSwitch.hpp"
#include "StepperMotor.hpp"

namespace po = boost::program_options;
using namespace moco;

CoffeeAutomatApplication::CoffeeAutomatApplication()
    : m_configuration(
          {Option("config-file", std::string("CoffeeMachine.json"), "Configuration file")})
{
}

bool CoffeeAutomatApplication::parseCommandLine(int argc, char const** argv)
{
    CommandLineParser parser(argc, argv);
    parser.add(m_configuration);
    return parser.parse();
}

bool CoffeeAutomatApplication::parseConfigurationFile()
{
    std::ifstream           inStream(m_configuration["config-file"].get<std::string>());
    ConfigurationFileParser parser(inStream);
    parser.add(m_configuration);

    return parser.parse();
}

bool CoffeeAutomatApplication::start(int argc, char const** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CoffeeAutomatService::setConfiguration(m_configuration);
    CupRotationTrayImpl::setConfiguration(m_configuration);
    HardwareAbstractionLayer::setConfiguration(m_configuration);

    LOG(info) << "Starting CoffeeAutomat application";
    bool success = parseCommandLine(argc, argv);

    if (success)
    {
        success = parseConfigurationFile();
    }

    if (success)
    {
        CommandMessageBroker                       commandMessageBroker;
        std::shared_ptr<IHardwareAbstractionLayer> hardwareAbstractionLayer =
            std::make_shared<HardwareAbstractionLayer>(
                m_configuration["hardware-abstraction-layer.gpio.logic-active-high"].get<bool>());
        IHardwareAbstractionLayer::init(hardwareAbstractionLayer);

        StepperMotor stepperMotor(
            m_configuration["cup-rotation-tray.stepper-motor.speed-rpm"].get<unsigned>(),
            m_configuration["cup-rotation-tray.stepper-motor.index"].get<unsigned>());
        CircularMotionController::PositionSwitchArray positionSwitcheArray = {
            std::make_shared<PositionSwitch>(
                m_configuration["cup-rotation-tray.position-switches.gpio-pin-1"].get<unsigned>()),
            std::make_shared<PositionSwitch>(
                m_configuration["cup-rotation-tray.position-switches.gpio-pin-2"].get<unsigned>()),
            std::make_shared<PositionSwitch>(
                m_configuration["cup-rotation-tray.position-switches.gpio-pin-3"].get<unsigned>())};
        CircularMotionController circularMotionController(stepperMotor, positionSwitcheArray);
        CupRotationTrayImpl      cupRotationTray(commandMessageBroker, circularMotionController);
        CoffeeAutomatImpl        coffeeAutomat(commandMessageBroker);
        CoffeeAutomatService     coffeeAutomatService(commandMessageBroker, m_configuration);
        success = coffeeAutomatService.start();
    }

    return success;
}
