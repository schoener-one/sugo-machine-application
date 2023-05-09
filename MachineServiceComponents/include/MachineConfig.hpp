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

#include "IConfiguration.hpp"

#include <chrono>
#include <string>

namespace sugo
{
namespace id
{
inline static const std::string MotorSpeedMax{"machine-control.motor-speed.max"};
inline static const std::string MotorSpeedDefault{"machine-control.motor-speed.default"};
inline static const std::string MotorSpeedIncrement{"machine-control.motor-speed.increment"};
inline static const std::string HeaterTemperatureMax{"machine-control.heater.max-temperature"};
inline static const std::string HeaterTemperatureMin{"machine-control.heater.min-temperature"};
inline static const std::string ObservationTimeoutGpioPin{
    "machine-control.observation-timeout.gpio-pin"};
inline static const std::string ObservationTimeoutTemperature{
    "machine-control.observation-timeout.temperature"};
inline static const std::string ObservationTimeoutTension{
    "machine-control.observation-timeout.tension"};
}  // namespace id

class MachineConfig final
{
public:
    static void addConfigurationOptions(IConfiguration& configuration);

private:
    MachineConfig() = default;  // Cannot be instanciated!
};
}  // namespace sugo