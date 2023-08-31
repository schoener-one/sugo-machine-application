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

#include <limits>
#include <ostream>
#include <string>
#include <vector>

#include "Common/IConfiguration.hpp"
#include "HardwareAbstractionLayer/GpioControl.hpp"
#include "HardwareAbstractionLayer/HalHelper.hpp"
#include "HardwareAbstractionLayer/HardwareAbstractionLayer.hpp"
#include "HardwareAbstractionLayer/StepperMotorControl.hpp"
#include "HardwareAbstractionLayer/TemperatureSensorControl.hpp"

using namespace sugo::hal;

void HardwareAbstractionLayer::finalize()
{
    postFinalization();  // TODO remove postInitialization mechanism!
}

bool HardwareAbstractionLayer::init(const common::IConfiguration& configuration)
{
    if (!initSubComponents<IGpioControl, GpioControl>(configuration, getId(), id::GpioControl,
                                                      m_gpioControllerMap))
    {
        LOG(error) << "Failed to initialize sub components: " << id::GpioControl;
        return false;
    }

    const auto& pinMap = m_gpioControllerMap.at(id::GpioControl)->getGpioPinMap();
    if ((pinMap.count(id::GpioPinMotorControlError) != 1) or
        (pinMap.count(id::GpioPinMotorControlReset) != 1))
    {
        LOG(error) << "Missing mandatory configuration entries";
        return false;
    }

    if (!initSubComponents<ITemperatureSensorControl, TemperatureSensorControl>(
            configuration, getId(), id::TemperatureSensorControl, m_temperatureSensorControllerMap,
            pinMap))
    {
        LOG(error) << "Failed to initialize sub components: " << id::TemperatureSensorControl;
        return false;
    }

    if (!initSubComponents<IStepperMotorControl, StepperMotorControl>(
            configuration, getId(), id::StepperMotorControl, m_stepperMotorControllerMap,
            *pinMap.at(id::GpioPinMotorControlError), *pinMap.at(id::GpioPinMotorControlReset)))
    {
        LOG(error) << "Failed to initialize sub components: " << id::StepperMotorControl;
        return false;
    }

    postInitialization();  // TODO remove postInitialization mechanism!
    return true;
}
