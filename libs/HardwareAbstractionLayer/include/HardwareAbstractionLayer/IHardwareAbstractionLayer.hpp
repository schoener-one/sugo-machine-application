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

#pragma once

#include <map>
#include <memory>
#include <string>

#include "HardwareAbstractionLayer/IGpioControl.hpp"
#include "HardwareAbstractionLayer/IHalObject.hpp"
#include "HardwareAbstractionLayer/IStepperMotorControl.hpp"
#include "HardwareAbstractionLayer/ITemperatureSensorControl.hpp"

namespace sugo::hal
{
/// @brief Interface class for the hardware abstraction layer
class IHardwareAbstractionLayer : public IHalObject
{
public:
    /// GPIO controller map
    using GpioControllerMap = std::map<Identifier, std::shared_ptr<IGpioControl>>;
    /// Stepper motor controller map
    using StepperMotorControllerMap = std::map<Identifier, std::shared_ptr<IStepperMotorControl>>;
    /// ADC input controller map
    using TemperatureSensorControllerMap =
        std::map<Identifier, std::shared_ptr<ITemperatureSensorControl>>;

    /**
     * @brief Returns a map of available GPIO controllers.
     *
     * @return const GpioControllerMap& Map of available controllers.
     */
    virtual const GpioControllerMap& getGpioControllerMap() const = 0;

    /**
     * @brief Returns a map of available stepper motor controllers.
     *
     * @return const StepperMotorControllerMap& Map of available controllers.
     */
    virtual const StepperMotorControllerMap& getStepperMotorControllerMap() const = 0;

    /**
     * @brief Returns a map of available ADC controllers.
     *
     * @return const TemperatureSensorControllerMap& Map of available controllers.
     */
    virtual const TemperatureSensorControllerMap& getTemperatureSensorControllerMap() const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
