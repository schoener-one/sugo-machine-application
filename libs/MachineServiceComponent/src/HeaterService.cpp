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

#include <cassert>
#include <limits>

#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/IHardwareAbstractionLayer.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/HeaterService.hpp"

using namespace sugo;
using namespace sugo::machine_service_component;

HeaterService::HeaterService(hal::Identifier heaterId, hal::Identifier temperatureSensorId,
                             machine_service_component::Identifier minTemperatureId,
                             machine_service_component::Identifier maxTemperatureId,
                             const common::ServiceLocator&         serviceLocator)
    : HardwareService(serviceLocator.get<hal::IHardwareAbstractionLayer>()),
      m_heaterId(std::move(heaterId)),
      m_temperatureSensorId(std::move(temperatureSensorId)),
      m_minTemperatureId(std::move(minTemperatureId)),
      m_maxTemperatureId(std::move(maxTemperatureId)),
      m_serviceLocator(serviceLocator)
{
}

bool HeaterService::switchHeater(bool switchOn)
{
    const auto& pinHeaterMerger = getGpioPin(m_heaterId);
    LOG(debug) << "Switch heater " << m_heaterId << " " << (switchOn ? "on" : "off");
    return pinHeaterMerger->setState(switchOn ? hal::IGpioPin::State::High
                                              : hal::IGpioPin::State::Low);
}

bool HeaterService::updateHeaterTemperature()
{
    auto&      temperatureSensor = getTemperatureSensor(m_temperatureSensorId);
    const auto result            = temperatureSensor->getTemperature();

    if (!result.has_value())
    {
        LOG(error) << "Failed to retrieve temperature";
        return false;
    }

    const auto value = std::move(result.value());
    assert(value.getUnit() == hal::Unit::Celcius);
    m_currentTemperature = value.getValue();
    LOG(debug) << "current temperature: " << m_currentTemperature << " °C";
    return true;
}

HeaterService::TemperatureState HeaterService::getHeaterTemperatureState()
{
    if (!updateHeaterTemperature())
    {
        return TemperatureState::ErrorNoTemperature;
    }

    const auto maxTemperature =
        m_serviceLocator.get<common::IConfiguration>().getOption(m_maxTemperatureId).get<int>();
    const auto minTemperature =
        m_serviceLocator.get<common::IConfiguration>().getOption(m_minTemperatureId).get<int>();

    if (m_currentTemperature >= maxTemperature)
    {
        return TemperatureState::AboveMaxTemperature;
    }
    else if (m_currentTemperature <= minTemperature)
    {
        return TemperatureState::BelowMinTemperature;
    }
    else
    {
        return TemperatureState::BetweenMinMaxTemperature;
    }
}
