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
/// @brief Configuration identifier type.
using Identifier = std::string;

namespace def
{
inline static constexpr unsigned ConfigMotorSpeedDefault                 = 50;
inline static constexpr unsigned ConfigMotorSpeedMax                     = 100;
inline static constexpr unsigned ConfigMotorSpeedIncrement               = 10;
inline static constexpr int      ConfigPreHeaterServiceTemperatureMax    = 145;
inline static constexpr int      ConfigPreHeaterServiceTemperatureMin    = 140;
inline static constexpr int      ConfigMergerHeaterServiceTemperatureMax = 205;
inline static constexpr int      ConfigMergerHeaterServiceTemperatureMin = 195;
inline static constexpr unsigned ConfigObservationTimeoutGpioPin         = 1000;
}  // namespace def

namespace description
{
inline static const Identifier ConfigMotorSpeedMax{"Maximum motor speed"};
inline static const Identifier ConfigMotorSpeedDefault{"Default motor speed"};
inline static const Identifier ConfigMotorSpeedIncrement{"Motor speed increment"};
inline static const Identifier ConfigPreHeaterServiceTemperatureMax{
    "Maximum pre-heater temperature"};
inline static const Identifier ConfigPreHeaterServiceTemperatureMin{
    "Minimum pre-heater temperature"};
inline static const Identifier ConfigMergerHeaterServiceTemperatureMax{
    "Maximum merger-heater temperature"};
inline static const Identifier ConfigMergerHeaterServiceTemperatureMin{
    "Minimum merger-heater temperature"};
inline static const Identifier ConfigObservationTimeoutGpioPin{"Observation timeout for GPIO pins"};
}  // namespace description

namespace id
{
inline static const Identifier ConfigMachineServiceComponent{"machine-service-component"};
inline static const Identifier ConfigMotorSpeed{ConfigMachineServiceComponent + ".motor-speed"};
inline static const Identifier ConfigMotorSpeedMax{ConfigMotorSpeed + ".max"};
inline static const Identifier ConfigMotorSpeedDefault{ConfigMotorSpeed + ".default"};
inline static const Identifier ConfigMotorSpeedIncrement{ConfigMotorSpeed + ".increment"};
inline static const Identifier ConfigPreHeaterService{ConfigMachineServiceComponent +
                                                      ".pre-heater"};
inline static const Identifier ConfigMergerHeaterService{ConfigMachineServiceComponent +
                                                         ".merger-heater"};
inline static const Identifier ConfigPreHeaterServiceTemperatureMax{ConfigPreHeaterService +
                                                                    ".max-temperature"};
inline static const Identifier ConfigPreHeaterServiceTemperatureMin{ConfigPreHeaterService +
                                                                    ".min-temperature"};
inline static const Identifier ConfigMergerHeaterServiceTemperatureMax{ConfigMergerHeaterService +
                                                                       ".max-temperature"};
inline static const Identifier ConfigMergerHeaterServiceTemperatureMin{ConfigMergerHeaterService +
                                                                       ".min-temperature"};
inline static const Identifier ConfigObservationTimeout{ConfigMachineServiceComponent +
                                                        ".observation-timeout"};
inline static const Identifier ConfigObservationTimeoutGpioPin{ConfigObservationTimeout +
                                                               ".gpio-pin"};
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