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

#include "MachineServiceComponentTest/MachineConfiguration.hpp"
#include "MachineServiceComponent/Configuration.hpp"

using namespace sugo;
using namespace sugo::common;
using namespace sugo::machine_service_component;
using namespace sugo::test;

using ::testing::ReturnRef;

unsigned MachineConfiguration::MotorSpeedDefault    = 50;
int32_t  MachineConfiguration::DefaultTemperature   = 25;
int      MachineConfiguration::HeaterTemperatureMax = 205;
int      MachineConfiguration::HeaterTemperatureMin = 180;
unsigned MachineConfiguration::MotorSpeedIncrement  = 10u;
unsigned MachineConfiguration::MotorSpeedMax        = 100u;
unsigned MachineConfiguration::ObservationTimeout   = 1000u;

void MachineConfiguration::prepareOptions(IConfigurationMock& mock)
{
    m_optionMotorSpeedDefault = {id::ConfigMotorSpeedDefault,
                                 static_cast<unsigned>(MotorSpeedDefault), ""};
    m_optionMotorSpeedMax     = {id::ConfigMotorSpeedMax, static_cast<unsigned>(MotorSpeedMax), ""};
    m_optionMotorSpeedIncrement        = {id::ConfigMotorSpeedIncrement,
                                   static_cast<unsigned>(MotorSpeedIncrement), ""};
    m_optionPreHeaterTemperatureMax    = {id::ConfigPreHeaterServiceTemperatureMax,
                                       static_cast<int>(HeaterTemperatureMax), ""};
    m_optionPreHeaterTemperatureMin    = {id::ConfigPreHeaterServiceTemperatureMin,
                                       static_cast<int>(HeaterTemperatureMin), ""};
    m_optionMergerHeaterTemperatureMax = {id::ConfigMergerHeaterServiceTemperatureMax,
                                          static_cast<int>(HeaterTemperatureMax), ""};
    m_optionMergerHeaterTemperatureMin = {id::ConfigMergerHeaterServiceTemperatureMin,
                                          static_cast<int>(HeaterTemperatureMin), ""};
    m_optionObservationTimeoutGpioPin  = {id::ConfigObservationTimeoutGpioPin,
                                         static_cast<unsigned>(ObservationTimeout), ""};

    ON_CALL(mock, getOption(id::ConfigMotorSpeedDefault))
        .WillByDefault(ReturnRef(m_optionMotorSpeedDefault));
    ON_CALL(mock, getOption(id::ConfigMotorSpeedMax))
        .WillByDefault(ReturnRef(m_optionMotorSpeedMax));
    ON_CALL(mock, getOption(id::ConfigMotorSpeedIncrement))
        .WillByDefault(ReturnRef(m_optionMotorSpeedIncrement));
    ON_CALL(mock, getOption(id::ConfigPreHeaterServiceTemperatureMax))
        .WillByDefault(ReturnRef(m_optionPreHeaterTemperatureMax));
    ON_CALL(mock, getOption(id::ConfigPreHeaterServiceTemperatureMin))
        .WillByDefault(ReturnRef(m_optionPreHeaterTemperatureMin));
    ON_CALL(mock, getOption(id::ConfigMergerHeaterServiceTemperatureMax))
        .WillByDefault(ReturnRef(m_optionMergerHeaterTemperatureMax));
    ON_CALL(mock, getOption(id::ConfigMergerHeaterServiceTemperatureMin))
        .WillByDefault(ReturnRef(m_optionMergerHeaterTemperatureMin));
    ON_CALL(mock, getOption(id::ConfigObservationTimeoutGpioPin))
        .WillByDefault(ReturnRef(m_optionObservationTimeoutGpioPin));
}
