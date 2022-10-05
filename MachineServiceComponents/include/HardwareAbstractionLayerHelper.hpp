///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-15
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IHardwareAbstractionLayer.hpp"
#include "MachineConfig.hpp"

namespace sugo
{
/**
 * @brief Get the Gpio pin according to the passed identifier.
 *
 * @param hal HAL object to be used.
 * @param idPin GPIO pin identifier.
 * @return auto& GPIO pin object.
 */
inline auto& getGpioPin(hal::IHardwareAbstractionLayer&    hal,
                        const hal::IHalObject::Identifier& idPin)
{
    assert(hal.getGpioControllerMap().count(config::GpioControlId) == 1);
    auto& gpioController = hal.getGpioControllerMap().at(config::GpioControlId);
    assert(gpioController->getGpioPinMap().count(idPin) == 1);
    auto& gpioPin = gpioController->getGpioPinMap().at(idPin);
    assert(gpioPin);
    return gpioPin;
}

/**
 * @brief Get the temperature sensor object according to the passed identifier.
 *
 * @param hal HAL object to be used.
 * @param idSensor Temperature sensor identifier.
 * @return auto& Temperature sensor object.
 */
inline auto& getTemperatureSensor(hal::IHardwareAbstractionLayer&    hal,
                                  const hal::IHalObject::Identifier& idSensor)
{
    assert(hal.getTemperatureSensorControllerMap().count(config::TemperatureSensorControlId) == 1);
    auto& temperatureSensorController =
        hal.getTemperatureSensorControllerMap().at(config::TemperatureSensorControlId);
    assert(temperatureSensorController->getTemperatureSensorMap().count(idSensor) == 1);
    auto& temperatureSensor = temperatureSensorController->getTemperatureSensorMap().at(idSensor);
    assert(temperatureSensor);
    return temperatureSensor;
}

/**
 * @brief Get the stepper motor object according to the passed identifier.
 *
 * @param hal HAL object to be used.
 * @param idSensor Stepper motor sensor identifier.
 * @return auto& Stepper motor object.
 */
inline auto& getStepperMotor(hal::IHardwareAbstractionLayer&    hal,
                             const hal::IHalObject::Identifier& idMotor)
{
    assert(hal.getStepperMotorControllerMap().count(config::StepperMotorControlId) == 1);
    auto& stepperMotorController = hal.getStepperMotorControllerMap().at(config::StepperMotorControlId);
    assert(stepperMotorController->getStepperMotorMap().count(idMotor) == 1);
    auto& stepperMotor = stepperMotorController->getStepperMotorMap().at(idMotor);
    assert(stepperMotor);
    return stepperMotor;
}
}  // namespace sugo
