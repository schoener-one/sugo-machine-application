///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-13
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IHalObject.hpp"

#include <chrono>
#include <string>

namespace sugo::config
{
// FIXME needs to be configurable in a configuration file!

static constexpr std::chrono::milliseconds TemperatureObservationPeriod(1000u);
static constexpr std::chrono::milliseconds MaxGpioPinObservationTimeout(1000u);

inline static const hal::Identifier GpioControlId{"gpio-control"};
inline static const hal::Identifier StepperMotorControlId{"stepper-motor-control"};
inline static const hal::Identifier TemperatureSensorControlId{"temperature-sensor-control"};
inline static const hal::Identifier GpioPinRelaySwitchFeederHeaterId{"relay-switch-heater-feeder"};
inline static const hal::Identifier GpioPinRelaySwitchMergerHeaterId{"relay-switch-heater-merger"};
inline static const hal::Identifier TemperatureSensorMergerId{"temperature-sensor-merger"};
inline static const hal::Identifier TemperatureSensorFeederId{"temperature-sensor-feeder"};
inline static const hal::Identifier GpioPinRelaySwitchLightRunId{"relay-switch-light-run"};
inline static const hal::Identifier GpioPinRelaySwitchLightPowerId{"relay-switch-light-power"};
inline static const hal::Identifier GpioPinRelaySwitchLightReadyId{"relay-switch-light-ready"};
inline static const hal::Identifier StepperMotorFeederId{"feeder"};
inline static const hal::Identifier StepperMotorCoilerId{"coiler"};
inline static const hal::Identifier GpioPinSignalFilamentTensionLowId{
    "signal-filament-tension-low"};
inline static const hal::Identifier GpioPinSignalFilamentTensionHighId{
    "signal-filament-tension-high"};

static constexpr unsigned MotorSpeedInc        = 10;                  ///< Default motor speed
static constexpr unsigned DefaultMotorSpeed    = 10 * MotorSpeedInc;  ///<  Default motor speed
static constexpr unsigned MinMotorSpeed        = 0;                   ///< Min motor speed
static constexpr unsigned MaxMotorSpeed        = 30 * MotorSpeedInc;  ///< Max motor speed
static constexpr int32_t  MaxHeaterTemperature = 205;  ///< Max heater temperature in Celcius
static constexpr int32_t  MinHeaterTemperature = 195;  ///< Min heater temperature in Celcius
}  // namespace sugo::config