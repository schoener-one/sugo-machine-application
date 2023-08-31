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

#include <gpiod.hpp>

#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/GpioPin.hpp"

using namespace sugo::hal;

GpioPin::~GpioPin()
{
    finalize();
}

bool GpioPin::init(const common::IConfiguration& configuration)
{
    if (m_line)
    {
        finalize();
    }

    const unsigned pin = configuration.getOption("pin").get<unsigned>();
    LOG(debug) << getId() << ": initializing gpio pin " << pin;

    m_line = m_chip.get_line(pin);

    if (!m_line || m_line.is_used())
    {
        LOG(error) << getId() << ": failed to initialize pin";
        return false;
    }

    const bool activeHigh = configuration.getOption(id::ActiveHigh).get<bool>();
    m_direction =
        (configuration.getOption(id::Direction).get<std::string>() == "in" ? Direction::In
                                                                           : Direction::Out);
    LOG(debug) << getId() << ": direction=" << m_direction;
    LOG(debug) << getId() << ": activate-high=" << activeHigh;

    gpiod::line_request lineConf = {
        getId().c_str(),
        (m_direction == Direction::In ? gpiod::line_request::EVENT_BOTH_EDGES
                                      : gpiod::line_request::DIRECTION_OUTPUT),
        ((activeHigh) ? 0 : gpiod::line_request::FLAG_ACTIVE_LOW)};

    m_line.request(lineConf);
    if (!m_line.is_requested())
    {
        LOG(error) << getId() << ": failed to request pin";
        finalize();
        return false;
    }

    const std::string defaultState = configuration.getOption("state").get<std::string>();
    if (m_direction == Direction::Out && !defaultState.empty())
    {
        State state = (defaultState == "high" ? State::High : State::Low);
        LOG(debug) << getId() << ": state=" << state;
        if (!setState(state))
        {
            LOG(error) << getId() << "failed to set default state";
            return false;
        }
    }

    if (m_direction == Direction::In)
    {
        LOG(debug) << getId() << ": state=" << (getState() == State::High ? "high" : "low");
    }

    return true;
}

void GpioPin::finalize()
{
    if (m_line)
    {
        LOG(debug) << getId() << ": finalizing gpio pin " << m_line.name();
        m_line.release();
        m_line.reset();
    }
}

IGpioPin::State GpioPin::getState() const
{
    return (m_line.get_value() == 0) ? State::Low : State::High;
}

bool GpioPin::setState(GpioPin::State state)
{
    assert(getDirection() == Direction::Out);
    m_line.set_value((State::Low == state) ? 0 : 1);
    return (state == getState());
}

IGpioPin::Direction GpioPin::getDirection() const
{
    return (m_line.direction() == gpiod::line::DIRECTION_INPUT) ? Direction::In : Direction::Out;
}

IGpioPin::Event GpioPin::waitForEvent(std::chrono::nanoseconds timeout)
{
    if (!m_line.event_wait(timeout))
    {
        return Event{timeout, EventType::Timeout};
    }

    auto event = m_line.event_read();
    return Event{event.timestamp, (event.event_type == gpiod::line_event::RISING_EDGE)
                                      ? EventType::RisingEdge
                                      : EventType::FallingEdge};
}
