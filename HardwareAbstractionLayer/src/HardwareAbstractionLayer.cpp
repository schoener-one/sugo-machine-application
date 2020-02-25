/*
 * HardwareAbstractionLayer.cpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#include <AdafruitStepperMotorController.hpp>
#include "HardwareAbstractionLayer.hpp"
#include "SysfsGpioController.hpp"

using namespace moco;

HardwareAbstractionLayer::HardwareAbstractionLayer()
:m_gpioController(std::make_unique<SysfsGpioController>())
,m_stepperMotorController(std::make_unique<AdafruitStepperMotorController>())
{
}

HardwareAbstractionLayer::~HardwareAbstractionLayer()
{
}
