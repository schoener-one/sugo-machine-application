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

#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>
#include <cassert>
#include <chrono>

#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/Simulator.hpp"

using namespace sugo::hal;

Simulator& Simulator::getInstance()
{
    static Simulator simulator;
    return simulator;
}

bool Simulator::start()
{
    m_startTime = Clock::now();
    return true;
}

void Simulator::stop()
{
    m_startTime.reset();
}

bool Simulator::isRunning() const
{
    return m_startTime.has_value();
}

void Simulator::registerGpioPin(const Identifier& id, IGpioPin::Direction direction)
{
    if (boost::starts_with(id, id::GpioPinRelaySwitchHeaterPrefix))
    {
        const std::string heaterTemperatureSensorId =
            id::TemperatureSensor + "-" + id.substr(id::GpioPinRelaySwitchHeaterPrefix.size());
        m_pins[id] = {
            IGpioPin::State::Low, direction,
            [this, heaterTemperatureSensorId](IGpioPin::State lastState, IGpioPin::State state) {
                if (lastState != state)
                {
                    this->switchHeater(heaterTemperatureSensorId, state);
                }
            }};
    }
    else
    {
        m_pins[id] = {IGpioPin::State::Low, direction, [](IGpioPin::State, IGpioPin::State) {}};
    }
}
void Simulator::unregisterGpioPin(const Identifier& id)
{
    std::lock_guard lock(m_mutex);
    auto            it = m_pins.find(id);

    if (it != m_pins.end())
    {
        m_pins.erase(it);
    }
}

IGpioPin::State Simulator::getState(const Identifier& id) const
{
    return m_pins.at(id).m_state;
}

bool Simulator::setState(const Identifier& id, IGpioPin::State state)
{
    LOG(debug) << "Simulation - set pin " << id << ": " << state;
    return m_pins.at(id).setState(state);
}

void Simulator::registerTemperatureSensor(const Identifier& id)
{
    m_temperatureSensors[id] = {false, Clock::now(), DefaultTemperature};
}

void Simulator::unregisterTemperatureSensor(const Identifier& id)
{
    std::lock_guard lock(m_mutex);
    auto            it = m_temperatureSensors.find(id);

    if (it != m_temperatureSensors.end())
    {
        m_temperatureSensors.erase(it);
    }
}

bool Simulator::GpioPin::setState(IGpioPin::State state)
{
    assert(m_direction == IGpioPin::Direction::Out);
    m_action(m_state, state);
    m_state = state;
    return true;
}

void Simulator::switchHeater(const Identifier& heaterTemperatureSensorId, IGpioPin::State state)
{
    std::lock_guard lock(m_mutex);

    if (m_temperatureSensors.count(heaterTemperatureSensorId) == 0)
    {
        LOG(warning) << "No heater found: " << heaterTemperatureSensorId;
        return;
    }

    auto& temperatureSensor = m_temperatureSensors.at(heaterTemperatureSensorId);

    if (state == IGpioPin::State::High)
    {
        LOG(debug) << "Simulation - start heating up for sensor " << heaterTemperatureSensorId;
        temperatureSensor.m_heatingUp = true;
    }
    else
    {
        LOG(debug) << "Simulation - start cooling down for sensor " << heaterTemperatureSensorId;
        temperatureSensor.m_heatingUp = false;
    }
    temperatureSensor.m_lastTemperatureChange = Clock::now();
}

ITemperatureSensor::Temperature Simulator::getTemperature(const Identifier& id) const
{
    const auto temperature = m_temperatureSensors.at(id).getTemperature();
    LOG(debug) << "Simulation - temperature on sensor " << id << ": " << temperature.getValue();
    return temperature;
}

ITemperatureSensor::Temperature Simulator::TemperatureSensor::getTemperature() const
{
    const auto secondsElapsed =
        std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - m_lastTemperatureChange)
            .count();
    const ITemperatureSensor::RawTemperature temperatureDiff =
        static_cast<ITemperatureSensor::RawTemperature>(secondsElapsed) * KelvinPerSecond;
    m_lastTemperatureChange = Clock::now();

    if (m_heatingUp)
    {
        m_curentTemperature =
            std::clamp(m_curentTemperature + temperatureDiff, DefaultTemperature, MaxTemperature);
        return ITemperatureSensor::Temperature{m_curentTemperature, Unit::Celcius};
    }
    else
    {
        m_curentTemperature =
            std::clamp(m_curentTemperature - temperatureDiff, DefaultTemperature, MaxTemperature);
        return ITemperatureSensor::Temperature{m_curentTemperature, Unit::Celcius};
    }
}