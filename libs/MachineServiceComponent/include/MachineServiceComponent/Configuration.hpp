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

#include <chrono>
#include <string>

#include "Common/IConfiguration.hpp"

namespace sugo::machine_service_component
{
namespace def
{
inline static constexpr unsigned ConfigMotorSpeedDefault             = 50;
inline static constexpr unsigned ConfigMotorSpeedMax                 = 100;
inline static constexpr unsigned ConfigMotorSpeedIncrement           = 10;
inline static constexpr int      ConfigHeaterTemperatureMax          = 205;
inline static constexpr int      ConfigHeaterTemperatureMin          = 195;
inline static constexpr unsigned ConfigObservationTimeoutGpioPin     = 1000;
inline static constexpr unsigned ConfigObservationTimeoutTemperature = 1000;
inline static constexpr unsigned ConfigObservationTimeoutTension     = 1000;
}  // namespace def

namespace description
{
inline static const std::string ConfigMotorSpeedMax{"Maximum motor speed"};
inline static const std::string ConfigMotorSpeedDefault{"Default motor speed"};
inline static const std::string ConfigMotorSpeedIncrement{"Motor speed increment"};
inline static const std::string ConfigHeaterTemperatureMax{"Maximum heater temperature"};
inline static const std::string ConfigHeaterTemperatureMin{"Minimum heater temperature"};
inline static const std::string ConfigObservationTimeoutGpioPin{
    "Observation timeout for GPIO pins"};
inline static const std::string ConfigObservationTimeoutTemperature{
    "Observation timeout for temperature values"};
inline static const std::string ConfigObservationTimeoutTension{
    "Observation timeout for filament tension values"};
}  // namespace description

namespace id
{
inline static const std::string ConfigMachineServiceComponent{"machine-service-component"};
inline static const std::string ConfigMotorSpeed{ConfigMachineServiceComponent + ".motor-speed"};
inline static const std::string ConfigMotorSpeedMax{ConfigMotorSpeed + ".max"};
inline static const std::string ConfigMotorSpeedDefault{ConfigMotorSpeed + ".default"};
inline static const std::string ConfigMotorSpeedIncrement{ConfigMotorSpeed + ".increment"};
inline static const std::string ConfigHeater{ConfigMachineServiceComponent + ".heater"};
inline static const std::string ConfigHeaterTemperatureMax{ConfigHeater + ".max-temperature"};
inline static const std::string ConfigHeaterTemperatureMin{ConfigHeater + ".min-temperature"};
inline static const std::string ConfigObservationTimeout{ConfigMachineServiceComponent +
                                                         ".observation-timeout"};
inline static const std::string ConfigObservationTimeoutGpioPin{ConfigObservationTimeout +
                                                                ".gpio-pin"};
inline static const std::string ConfigObservationTimeoutTemperature{ConfigObservationTimeout +
                                                                    ".temperature"};
inline static const std::string ConfigObservationTimeoutTension{ConfigObservationTimeout +
                                                                ".tension"};
}  // namespace id

namespace config
{
/**
 * @brief Adds machine service component configuration options.
 *
 * @param configuration Configuration object.
 */
void addConfigurationOptions(common::IConfiguration& configuration);
}  // namespace config
}  // namespace sugo::machine_service_component