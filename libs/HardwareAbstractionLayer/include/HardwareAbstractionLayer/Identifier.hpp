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

#include <string>

namespace sugo::hal
{
/// @brief HAL object identifier type
using Identifier = std::string;

namespace id
{
// out pins
inline const Identifier GpioPinRelaySwitchFanFeeder{"relay-switch-fan-feeder"};
inline const Identifier GpioPinRelaySwitchFanMerger{"relay-switch-fan-merger"};
inline const Identifier GpioPinRelaySwitchHeaterPrefix{"relay-switch-heater-"};
inline const Identifier GpioPinRelaySwitchHeaterFeeder{GpioPinRelaySwitchHeaterPrefix + "feeder"};
inline const Identifier GpioPinRelaySwitchHeaterMerger{GpioPinRelaySwitchHeaterPrefix + "merger"};
inline const Identifier GpioPinRelaySwitchLightRun{"relay-switch-light-run"};
inline const Identifier GpioPinRelaySwitchLightPower{"relay-switch-light-power"};
inline const Identifier GpioPinRelaySwitchLightReady{"relay-switch-light-ready"};
inline const Identifier GpioPinTemperatureSensorControlCsFeeder{
    "temperature-sensor-control-cs-feeder"};
inline const Identifier GpioPinTemperatureSensorControlCsMerger{
    "temperature-sensor-control-cs-merger"};
inline const Identifier GpioPinMotorControlReset{"motor-control-reset"};
// in pins
inline const Identifier GpioPinSignalFilamentTensionLow{"signal-filament-tension-low"};
inline const Identifier GpioPinSignalFilamentTensionHigh{"signal-filament-tension-high"};
inline const Identifier GpioPinSignalFilamentTensionOverload{"signal-filament-tension-overload"};
inline const Identifier GpioPinSignalButtonStop{"signal-button-stop"};
inline const Identifier GpioPinMotorControlError{"motor-control-error"};
inline const Identifier HardwareAbstractionLayer{"hardware-abstraction-layer"};
inline const Identifier GpioControl{"gpio-control"};
inline const Identifier StepperMotorControl{"stepper-motor-control"};
inline const Identifier TemperatureSensorControl{"temperature-sensor-control"};
inline const Identifier TemperatureSensor{"temperature-sensor"};
inline const Identifier TemperatureSensorMerger{TemperatureSensor + "-merger"};
inline const Identifier TemperatureSensorFeeder{TemperatureSensor + "-feeder"};
inline const Identifier StepperMotorFeeder{"feeder"};
inline const Identifier StepperMotorCoiler{"coiler"};
inline const Identifier GpioPin{"gpio-pin"};
inline const Identifier ChipSelect{"chip-select"};
inline const Identifier ActiveHigh{"active-high"};
inline const Identifier MaxSpeedRpm{"max-speed-rpm"};
inline const Identifier I2cAddress{"i2c-address"};
inline const Identifier Direction{"direction"};
}  // namespace id
}  // namespace sugo::hal