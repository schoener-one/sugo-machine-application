/*
 * HardwareAbstractionLayerImpl.cpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#include "HardwareAbstractionLayer.hpp"
#include "Configuration.hpp"
#include "GpioController.hpp"

using namespace sugo::hal;

bool HardwareAbstractionLayer::init(const IConfiguration& configuration)
{
    for (auto& option : configuration.getOptionList("hardware-abstraction-layer.gpio-controllers."))
    {
        LOG(debug) << option.getName();
        // m_gpioControllerMap.insert(option.getName(), std::make_unique<GpioPin>();
    }
}
