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
                             const common::ServiceLocator& serviceLocator)
    : HardwareService(serviceLocator.get<hal::IHardwareAbstractionLayer>()),
      m_heaterId(std::move(heaterId)),
      m_temperatureSensorId(std::move(temperatureSensorId)),
      m_serviceLocator(serviceLocator),
      m_temperatureObserver(
          std::chrono::milliseconds(m_serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigObservationTimeoutTemperature)
                                        .get<unsigned>()),
          [&]() { updateHeaterTemperatureAndCheck(); }, m_heaterId + "common::Timer"),
      m_lastCheckedTemperature(std::numeric_limits<Temperature>::min())
{
}

bool HeaterService::switchHeater(bool switchOn)
{
    const auto& pinHeaterMerger = getGpioPin(m_heaterId);
    LOG(debug) << "Switch heater " << m_heaterId << " " << (switchOn ? "on" : "off");
    return pinHeaterMerger->setState(switchOn ? hal::IGpioPin::State::High
                                              : hal::IGpioPin::State::Low);
}

void HeaterService::updateHeaterTemperature()
{
    auto& temperatureSensor = getTemperatureSensor(m_temperatureSensorId);
    auto  value             = temperatureSensor->getTemperature();
    assert(value.getUnit() == hal::Unit::Celcius);
    m_currentTemperature = value.getValue();
}

void HeaterService::updateHeaterTemperatureAndCheck()
{
    updateHeaterTemperature();

    if (m_lastCheckedTemperature != m_currentTemperature)
    {
        // Implement hysteresis
        if (m_currentTemperature >= m_serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigHeaterTemperatureMax)
                                        .get<int>())
        {
            LOG(debug) << "Max temperature reached: " << m_lastCheckedTemperature << "/"
                       << m_currentTemperature;
            onTemperatureLimitEvent(TemperatureLimitEvent::MaxTemperatureReached);
        }
        else if (m_currentTemperature <= m_serviceLocator.get<common::IConfiguration>()
                                             .getOption(id::ConfigHeaterTemperatureMin)
                                             .get<int>())
        {
            LOG(debug) << "Min temperature reached: " << m_lastCheckedTemperature << "/"
                       << m_currentTemperature;
            onTemperatureLimitEvent(TemperatureLimitEvent::MinTemperatureReached);
        }

        m_lastCheckedTemperature = m_currentTemperature;
    }
}

bool HeaterService::startTemperatureObservation()
{
    return m_temperatureObserver.start();
}

void HeaterService::stopTemperatureObservation()
{
    m_temperatureObserver.stop();
}