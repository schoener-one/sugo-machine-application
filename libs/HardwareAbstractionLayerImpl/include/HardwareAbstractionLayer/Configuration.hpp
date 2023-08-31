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

#include <vector>

#include "Common/IConfiguration.hpp"
#include "HardwareAbstractionLayer/Identifier.hpp"

namespace sugo::hal
{
namespace def
{
inline static const std::string              ConfigGpioControlDevice{"gpiochip0"};
inline static const std::string              ConfigStepperMotorControlDevice{"i2c-1"};
inline static const std::vector<std::string> ConfigGpioPinEnabled{};
inline static const std::vector<std::string> ConfigStepperMotorEnabled{};
inline static const std::string              ConfigTemperatureSensorDevice{"spidev0.0"};
inline static const std::vector<std::string> ConfigTemperatureSensorEnabled{};
}  // namespace def

namespace id
{
inline static const std::string ConfigHardwareAbstractionLayer{"hardware-abstraction-layer"};
inline static const std::string ConfigDevice{".device"};

inline static const std::string ConfigGpioControl{ConfigHardwareAbstractionLayer + "." +
                                                  GpioControl};
inline static const std::string ConfigGpioControlDevice{ConfigGpioControl + ConfigDevice};
inline static const std::string ConfigGpioPin{ConfigGpioControl + "." + GpioPin};
inline static const std::string ConfigGpioPinEnabled{ConfigGpioPin + "-enabled"};
inline static const std::string ConfigStepperMotorControl{ConfigHardwareAbstractionLayer + "." +
                                                          StepperMotorControl};
inline static const std::string ConfigStepperMotor{ConfigStepperMotorControl + ".motor"};
inline static const std::string ConfigStepperMotorEnabled{ConfigStepperMotor + "-enabled"};
inline static const std::string ConfigStepperMotorControlDevice{ConfigStepperMotorControl +
                                                                ConfigDevice};
inline static const std::string ConfigTemperatureSensorControl{ConfigHardwareAbstractionLayer +
                                                               "." + TemperatureSensorControl};
inline static const std::string ConfigTemperatureSensorDevice{ConfigTemperatureSensorControl +
                                                              ConfigDevice};
inline static const std::string ConfigTemperatureSensor{ConfigTemperatureSensorControl + "." +
                                                        TemperatureSensor};
inline static const std::string ConfigTemperatureSensorEnabled{ConfigTemperatureSensor +
                                                               "-enabled"};
}  // namespace id

namespace description
{
inline static const std::string ConfigGpioControlDevice{"GPIO device name"};
inline static const std::string ConfigStepperMotorControlDevice{"Motor control I2C device name"};
inline static const std::string ConfigGpioPinEnabled{"List of GPIO pin names to be enabled"};
inline static const std::string ConfigStepperMotorEnabled{
    "List of stepper-motor names to be enabled"};
inline static const std::string ConfigTemperatureSensorDevice{"ADC device name"};
inline static const std::string ConfigTemperatureSensorEnabled{
    "List of temperature sensors enabled"};
}  // namespace description

namespace config
{
/**
 * @brief Adds hardware abstraction layer configuration options.
 *
 * @param configuration Configuration object.
 */
void addConfigurationOptions(common::IConfiguration& configuration);
}  // namespace config
}  // namespace sugo::hal