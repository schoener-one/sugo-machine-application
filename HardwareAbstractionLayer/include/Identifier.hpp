///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-07
 */
///////////////////////////////////////////////////////////////////////////////

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
inline const Identifier GpioPinTemperatureSensorControlCsFeeder{"temperature-sensor-control-cs-feeder"};
inline const Identifier GpioPinTemperatureSensorControlCsMerger{"temperature-sensor-control-cs-merger"};
inline const Identifier GpioPinMotorControlReset{"motor-control-reset"};
// in pins
inline const Identifier GpioPinSignalFilamentTensionLow{"signal-filament-tension-low"};
inline const Identifier GpioPinSignalFilamentTensionHigh{"signal-filament-tension-high"};
inline const Identifier GpioPinSignalButtonStart{"signal-button-start"};
inline const Identifier GpioPinSignalButtonStop{"signal-button-stop"};
inline const Identifier GpioPinMotorControlError{"motor-control-error"};

inline const Identifier HardwareAbstractionLayer{"hardware-abstraction-layer"};
inline const Identifier GpioControl{"gpio-control"};
inline const Identifier StepperMotorControl{"stepper-motor-control"};
inline const Identifier TemperatureSensorControl{"temperature-sensor-control"};
inline const Identifier TemperatureSensorPrefix{"temperature-sensor-"};
inline const Identifier TemperatureSensorMerger{TemperatureSensorPrefix + "merger"};
inline const Identifier TemperatureSensorFeeder{TemperatureSensorPrefix + "feeder"};
inline const Identifier StepperMotorFeeder{"feeder"};
inline const Identifier StepperMotorCoiler{"coiler"};
}  // namespace id
namespace option::id
{
inline const Identifier GpioPin{"gpio-pin"};
inline const Identifier TemperatureSensor{"temperature-sensor"};
inline const Identifier ChipSelect{"chip-select"};
inline const Identifier ActiveHigh{"active-high"};
inline const Identifier MaxSpeedRpm{"max-speed-rpm"};
inline const Identifier I2cAddress{"i2c-address"};
}  // namespace option::id
}  // namespace sugo::hal