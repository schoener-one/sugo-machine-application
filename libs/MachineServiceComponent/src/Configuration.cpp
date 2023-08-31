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

#include "MachineServiceComponent/Configuration.hpp"

using namespace sugo;
using namespace sugo::machine_service_component;

void config::addConfigurationOptions(sugo::common::IConfiguration& configuration)
{
    configuration.add(common::Option(id::ConfigMotorSpeedDefault, def::ConfigMotorSpeedDefault,
                                     description::ConfigMotorSpeedDefault));
    configuration.add(common::Option(id::ConfigMotorSpeedMax, def::ConfigMotorSpeedMax,
                                     description::ConfigMotorSpeedMax));
    configuration.add(common::Option(id::ConfigMotorSpeedIncrement, def::ConfigMotorSpeedIncrement,
                                     description::ConfigMotorSpeedIncrement));
    configuration.add(common::Option(id::ConfigHeaterTemperatureMax,
                                     def::ConfigHeaterTemperatureMax,
                                     description::ConfigHeaterTemperatureMax));
    configuration.add(common::Option(id::ConfigHeaterTemperatureMin,
                                     def::ConfigHeaterTemperatureMin,
                                     description::ConfigHeaterTemperatureMin));
    configuration.add(common::Option(id::ConfigObservationTimeoutGpioPin,
                                     def::ConfigObservationTimeoutGpioPin,
                                     description::ConfigObservationTimeoutTemperature));
    configuration.add(common::Option(id::ConfigObservationTimeoutTemperature,
                                     def::ConfigObservationTimeoutTemperature,
                                     description::ConfigObservationTimeoutTemperature));
    configuration.add(common::Option(id::ConfigObservationTimeoutTension,
                                     def::ConfigObservationTimeoutTension,
                                     description::ConfigObservationTimeoutTension));
}