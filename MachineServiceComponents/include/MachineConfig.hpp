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

#include <chrono>
#include <string>

namespace sugo::config
{
// FIXME needs to be configurable in a configuration file!

static constexpr std::chrono::milliseconds TemperatureObservationPeriod(1000u);

inline static const std::string GpioControlId{"gpio-control"};
inline static const std::string StepperMotorControlId{"stepper-motor-control"};
inline static const std::string TemperatureSensorControlId{"temperature-sensor-control"};

inline static const std::string GpioPinRelaySwitchFeederHeaterId{"relay-switch-heater-feeder"};
inline static const std::string GpioPinRelaySwitchMergerHeaterId{"relay-switch-heater-merger"};
inline static const std::string TemperatureSensorMergerId{"temperature-sensor-merger"};
inline static const std::string TemperatureSensorFeederId{"temperature-sensor-feeder"};
inline static const std::string GpioPinRelaySwitchLightRunId{"relay-switch-light-run"};
inline static const std::string GpioPinRelaySwitchLightPowerId{"relay-switch-light-power"};
inline static const std::string GpioPinRelaySwitchLightReadyId{"relay-switch-light-ready"};
inline static const std::string StepperMotorFeederId{"feeder"};
inline static const std::string StepperMotorCoilerId{"coiler"};

static constexpr unsigned MotorSpeedInc        = 10;                  ///< Default motor speed
static constexpr unsigned DefaultMotorSpeed    = 10 * MotorSpeedInc;  ///<  Default motor speed
static constexpr unsigned MaxMotorSpeed        = 30 * MotorSpeedInc;  ///< Max motor speed
static constexpr int32_t  MaxHeaterTemperature = 205;  ///< Max heater temperature in Celcius
static constexpr int32_t  MinHeaterTemperature = 195;  ///< Min heater temperature in Celcius
}  // namespace sugo::config