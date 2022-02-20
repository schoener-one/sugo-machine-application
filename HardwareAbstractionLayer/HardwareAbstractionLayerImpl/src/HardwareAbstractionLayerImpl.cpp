/*
 * HardwareAbstractionLayerImpl.cpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#include "HardwareAbstractionLayerImpl.hpp"
#include "SysfsGpioController.hpp"

using namespace sugo;

void HardwareAbstractionLayerImpl::init(const IConfiguration& configuration)
    m_gpioController = std::make_unique<SysfsGpioController>(
        configuration["hardware-abstraction-layer.gpio.logic-active-high"].get<bool>());
{
}

HardwareAbstractionLayerImpl::~HardwareAbstractionLayerImpl() {}
