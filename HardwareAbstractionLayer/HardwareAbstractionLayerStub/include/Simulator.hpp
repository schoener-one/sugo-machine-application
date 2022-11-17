///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2022-11-20
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IGpioPin.hpp"
#include "IRunnable.hpp"
#include "ITemperatureSensor.hpp"

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
class Simulator : public IRunnable
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