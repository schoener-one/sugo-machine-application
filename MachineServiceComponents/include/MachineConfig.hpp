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

static constexpr unsigned MotorSpeedInc        = 10;                  ///< Default motor speed
static constexpr unsigned DefaultMotorSpeed    = 10 * MotorSpeedInc;  ///<  Default motor speed
static constexpr unsigned MinMotorSpeed        = 0;                   ///< Min motor speed
static constexpr unsigned MaxMotorSpeed        = 30 * MotorSpeedInc;  ///< Max motor speed
static constexpr int32_t  MaxHeaterTemperature = 205;  ///< Max heater temperature in Celcius
static constexpr int32_t  MinHeaterTemperature = 195;  ///< Min heater temperature in Celcius
}  // namespace sugo::config