/*
 * CoffeeMachineApplication.cpp
 *
 *  Created on: 01.12.2019
 *      Author: denis
 */

#include "CoffeeAutomatApplication.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "CupRotationTray.hpp"
#include "CircularMotionController.hpp"
#include "CoffeeAutomat.hpp"
#include "CoffeeAutomatService.hpp"
#include "Globals.hpp"
#include "HardwareAbstractionLayer.hpp"
#include "ICommandMessageBroker.hpp"
#include "PositionSwitch.hpp"
#include "StepperMotor.hpp"
#ifdef TARGET_X86
#include "HardwareAbstractionLayerStub.hpp"
#endif  // TARGET_X86

namespace po = boost::program_options;
using namespace moco;

namespace
{
const std::string serviceAddressId = "service-address";
const std::string serviceAddress   = "*";
const std::string servicePortId    = "service-port";
const std::string servicePort      = "1234";
const std::string configFile       = "config-file";
const std::string cupRotationTraySmControllerMotorIndexId =
    "cup-rotation-tray.sm-controller.motor-index";
const unsigned    cupRotationTraySmControllerMotorIndex = StepperMotor::DefaultMotorIndex;
const std::string cupRotationTraySmControllerMotorSpeedId =
    "cup-rotation-tray.sm-controller.motor-speed-rpm";
const unsigned    cupRotationTraySmControllerMotorSpeed = StepperMotor::DefaultSpeedRpm;
const std::string cupRotationTrayPositionSwitch1Id =
    "cup-rotation-tray.position-switches.gpio-pin-1";
const unsigned    cupRotationTrayPositionSwitch1 = 1;
const std::string cupRotationTrayPositionSwitch2Id =
    "cup-rotation-tray.position-switches.gpio-pin-2";
const unsigned    cupRotationTrayPositionSwitch2 = 2;
const std::string cupRotationTrayPositionSwitch3Id =
    "cup-rotation-tray.position-switches.gpio-pin-3";
const unsigned    cupRotationTrayPositionSwitch3 = 3;
const std::string cupRotationTrayPositionSwitchesGpioLogicId =
    "cup-rotation-tray.position-switches.gpio-logic";
const std::string cupRotationTrayPositionSwitchesGpioLogic = "active-high";
}  // namespace

CoffeeAutomatApplication::CoffeeAutomatApplication() : m_initSuccessfully(false) {}

void CoffeeAutomatApplication::parseOptions(int argc, char** argv)
{
    m_initSuccessfully = true;
    // Parse commandline options
    po::options_description commandLineOpts("Allowed commandline options");
    commandLineOpts.add_options()("help", "this help message")(
        serviceAddressId.c_str(), po::value<std::string>()->default_value(serviceAddress),
        "network address of the service")(servicePortId.c_str(),
                                          po::value<std::string>()->default_value(servicePort),
                                          "network port of the service")(
        configFile.c_str(), po::value<std::string>()->default_value(""), "configuration file");

    po::store(po::parse_command_line(argc, argv, commandLineOpts), m_vm);
    po::notify(m_vm);

    if (m_vm.count("help"))
    {
        std::cout << commandLineOpts << "\n";
        m_initSuccessfully = false;
    }

    if (m_initSuccessfully && !m_vm[configFile].empty())
    {
        po::options_description configFileOpts("Allowed configuration file options");
        // clang-format off
		configFileOpts.add_options()
			(cupRotationTraySmControllerMotorIndexId.c_str(),
					po::value<unsigned>()->default_value(cupRotationTraySmControllerMotorIndex))
			(cupRotationTraySmControllerMotorSpeedId.c_str(),
					po::value<unsigned>()->default_value(cupRotationTraySmControllerMotorSpeed))
			(cupRotationTrayPositionSwitch1Id.c_str(),
					po::value<unsigned>()->default_value(cupRotationTrayPositionSwitch1))
			(cupRotationTrayPositionSwitch2Id.c_str(),
					po::value<unsigned>()->default_value(cupRotationTrayPositionSwitch2))
			(cupRotationTrayPositionSwitch3Id.c_str(),
					po::value<unsigned>()->default_value(cupRotationTrayPositionSwitch3))
			(cupRotationTrayPositionSwitchesGpioLogicId.c_str(),
					po::value<std::string>()->default_value(cupRotationTrayPositionSwitchesGpioLogic))
		;
        // clang-format on
        std::ifstream     file;
        const std::string fileName = m_vm[configFile].as<std::string>();
        file.open(fileName);
        if (!file.fail())
        {
            po::store(po::parse_config_file(file, configFileOpts, true), m_vm);
            po::notify(m_vm);
        }
        else
        {
            LOG(error) << "Configuration file '" << fileName << "' not found!";
            m_initSuccessfully = false;
        }
    }
}

int CoffeeAutomatApplication::start()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    LOG(info) << "Starting CoffeeAutomat application";
    int rv = 0;

    if (m_initSuccessfully)
    {
        // Create HAL
        std::shared_ptr<IHardwareAbstractionLayer> hardwareAbstractionLayer =
#ifdef TARGET_X86
            std::make_shared<HardwareAbstractionLayerStub>();
#else
            std::make_shared<HardwareAbstractionLayer>();
#endif  // TARGET_X86
        HardwareAbstractionLayer::init(hardwareAbstractionLayer);

        CommandMessageBroker commandMessageBroker;

        StepperMotor stepperMotor(m_vm[cupRotationTraySmControllerMotorSpeedId].as<unsigned>(),
                                  m_vm[cupRotationTraySmControllerMotorIndexId].as<unsigned>());
        const bool   isGpioActiveHigh =
            (m_vm[cupRotationTrayPositionSwitchesGpioLogicId].as<std::string>() == "active-high");
        CircularMotionController::PositionSwitchArray positionSwitcheArray = {
            std::make_shared<PositionSwitch>(m_vm[cupRotationTrayPositionSwitch1Id].as<unsigned>(),
                                             isGpioActiveHigh),
            std::make_shared<PositionSwitch>(m_vm[cupRotationTrayPositionSwitch2Id].as<unsigned>(),
                                             isGpioActiveHigh),
            std::make_shared<PositionSwitch>(m_vm[cupRotationTrayPositionSwitch3Id].as<unsigned>(),
                                             isGpioActiveHigh)};
        CircularMotionController circularMotionController(
            stepperMotor, positionSwitcheArray);
        CupRotationTrayImpl cupRotationTray(commandMessageBroker, circularMotionController);
        CoffeeAutomatImpl   coffeeAutomat(commandMessageBroker);

        const std::string address =
            m_vm[serviceAddressId].as<std::string>() + ":" + m_vm[servicePortId].as<std::string>();
        CoffeeAutomatService coffeeAutomatService(address, commandMessageBroker);
        (void)coffeeAutomatService.start();
    }
    else
    {
        rv = -1;
    }
    return rv;
}
