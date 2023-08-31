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

#include "Common/IRunnable.hpp"
#include "HardwareAbstractionLayer/IGpioPin.hpp"
#include "HardwareAbstractionLayer/ITemperatureSensor.hpp"

#include <chrono>
#include <map>
#include <mutex>
#include <optional>

namespace sugo::hal
{
/**
 * @brief Class to simulate the hardware behaviour
 *
 * @note Implements the singleton pattern
 */
class Simulator : public common::IRunnable
{
public:
    /// @brief Used clock type
    using Clock = std::chrono::steady_clock;

    /**
     * @brief Returns the instance of the hardware simulator
     *
     * @return Instance of the simulator
     */
    static Simulator& getInstance();

    bool start() override;
    void stop() override;
    bool isRunning() const override;

    void            registerGpioPin(const Identifier& id, IGpioPin::Direction direction);
    void            unregisterGpioPin(const Identifier& id);
    IGpioPin::State getState(const Identifier& id) const;
    bool            setState(const Identifier& id, IGpioPin::State state);

    void                            registerTemperatureSensor(const Identifier& id);
    void                            unregisterTemperatureSensor(const Identifier& id);
    ITemperatureSensor::Temperature getTemperature(const Identifier& id) const;

private:
    Simulator() = default;

    static constexpr ITemperatureSensor::RawTemperature KelvinPerSecond    = 20;
    static constexpr ITemperatureSensor::RawTemperature MaxTemperature     = 220;
    static constexpr ITemperatureSensor::RawTemperature DefaultTemperature = 25;

    struct GpioPin
    {
        IGpioPin::State     m_state     = IGpioPin::State::Low;
        IGpioPin::Direction m_direction = IGpioPin::Direction::In;

        bool setState(IGpioPin::State state);
        using Action = std::function<void(IGpioPin::State lastState, IGpioPin::State state)>;
        Action m_action;
    };

    struct TemperatureSensor
    {
        bool                                       m_heatingUp = false;
        mutable Clock::time_point                  m_lastTemperatureChange;
        mutable ITemperatureSensor::RawTemperature m_curentTemperature = DefaultTemperature;

        ITemperatureSensor::Temperature getTemperature() const;
    };

    void switchHeater(const Identifier& heaterTemperatureSensorId, IGpioPin::State state);

    using PinMap  = std::map<Identifier, GpioPin>;
    PinMap m_pins = {};

    using TemperatureSensorMap                = std::map<Identifier, TemperatureSensor>;
    TemperatureSensorMap m_temperatureSensors = {};

    std::optional<Clock::time_point> m_startTime;
    std::mutex                       m_mutex;
};
}  // namespace sugo::hal