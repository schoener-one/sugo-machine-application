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

#include <cassert>

#include "HardwareAbstractionLayer/IHardwareAbstractionLayer.hpp"
#include "MachineServiceComponent/Configuration.hpp"

namespace sugo::machine_service_component
{
// TODO move to hardware service module!

/// @brief Base class for all hardware service layer classes.
class HardwareService
{
public:
    /// @brief Default destructor.
    virtual ~HardwareService() = default;

    /// @brief Default copy constructor.
    HardwareService(const HardwareService&) = default;

    /// @brief Default move constructor.
    HardwareService(HardwareService&&) = default;

    /// @brief Default assignment operator.
    HardwareService& operator=(const HardwareService&) = default;

    /// @brief Default move operator.
    HardwareService& operator=(HardwareService&&) = default;

protected:
    explicit HardwareService(const hal::IHardwareAbstractionLayer& hal) : m_hal(hal)
    {
    }

    /**
     * @brief Get the Gpio pin according to the passed identifier.
     *
     * @param idPin GPIO pin identifier.
     * @return auto& GPIO pin object.
     */
    auto& getGpioPin(const hal::Identifier& idPin)
    {
        assert(m_hal.getGpioControllerMap().count(hal::id::GpioControl) == 1);
        auto& gpioController = m_hal.getGpioControllerMap().at(hal::id::GpioControl);
        assert(gpioController->getGpioPinMap().count(idPin) == 1);
        auto& gpioPin = gpioController->getGpioPinMap().at(idPin);
        assert(gpioPin);
        return gpioPin;
    }

    /**
     * @brief Get the temperature sensor object according to the passed identifier.
     *
     * @param sensorId Temperature sensor identifier.
     * @return auto& Temperature sensor object.
     */
    auto& getTemperatureSensor(const hal::Identifier& sensorId)
    {
        assert(m_hal.getTemperatureSensorControllerMap().count(hal::id::TemperatureSensorControl) ==
               1);
        auto& temperatureSensorController =
            m_hal.getTemperatureSensorControllerMap().at(hal::id::TemperatureSensorControl);
        assert(temperatureSensorController->getTemperatureSensorMap().count(sensorId) == 1);
        auto& temperatureSensor =
            temperatureSensorController->getTemperatureSensorMap().at(sensorId);
        assert(temperatureSensor);
        return temperatureSensor;
    }

    /**
     * @brief Get the stepper motor object according to the passed identifier.
     *
     * @param sensorId Stepper motor sensor identifier.
     * @return auto& Stepper motor object.
     */
    auto& getStepperMotor(const hal::Identifier& idMotor)
    {
        assert(m_hal.getStepperMotorControllerMap().count(hal::id::StepperMotorControl) == 1);
        auto& stepperMotorController =
            m_hal.getStepperMotorControllerMap().at(hal::id::StepperMotorControl);
        assert(stepperMotorController->getStepperMotorMap().count(idMotor) == 1);
        auto& stepperMotor = stepperMotorController->getStepperMotorMap().at(idMotor);
        assert(stepperMotor);
        return stepperMotor;
    }

private:
    const hal::IHardwareAbstractionLayer& m_hal;  ///< HAL object.
};
}  // namespace sugo::machine_service_component
