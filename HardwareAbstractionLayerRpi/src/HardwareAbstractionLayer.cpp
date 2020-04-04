/*
 * HardwareAbstractionLayer.cpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#include "HardwareAbstractionLayer.hpp"
#include "AdafruitStepperMotorController.hpp"
#include "SysfsGpioController.hpp"

using namespace moco;

void HardwareAbstractionLayer::setConfiguration(IConfiguration& configuration)
{
    configuration.add(Option("hardware-abstraction-layer.gpio.logic-active-high", false,
                             "HAL: Indicates if GPIO logic is active-high or -low"));
}

HardwareAbstractionLayer::HardwareAbstractionLayer(bool isActiveHigh)
    : m_gpioController(std::make_unique<SysfsGpioController>(isActiveHigh)),
      m_stepperMotorController(std::make_unique<AdafruitStepperMotorController>())
{
}

HardwareAbstractionLayer::~HardwareAbstractionLayer() {}
