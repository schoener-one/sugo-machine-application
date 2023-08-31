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
#include <ostream>

#include "HardwareAbstractionLayer/IHalObject.hpp"

namespace sugo::hal
{
/// @brief Interface class for GPIO pins
class IGpioPin : public IHalObject
{
public:
    /// Indicates an invalid pin
    constexpr static unsigned InvalidPin = 0xffffffff;

    /**
     * @brief GPIO pin state
     *
     */
    enum State
    {
        Low = 0,  ///< Low state
        High      ///< High state
    };

    /**
     * @brief GPIO pin direction
     *
     */
    enum Direction
    {
        In,  ///< Input direction
        Out  ///< Output direction
    };

    /**
     * @brief Type of an GPIO event
     *
     */
    enum EventType
    {
        Timeout,     ///< Timeout event
        RisingEdge,  ///< Rising edge event
        FallingEdge  ///< Falling edge event
    };

    /**
     * @brief Gpio event object
     *
     */
    struct Event
    {
        /// Time when the event occurred.
        std::chrono::nanoseconds timestamp = std::chrono::nanoseconds(0);
        /// Type of the event.
        EventType type = EventType::Timeout;
    };

    /**
     * @brief Returns the state of the pin.
     *
     * @return State of the pin
     */
    virtual State getState() const = 0;

    /**
     * @brief Set the State of the GPIO
     *
     * @param state
     * @return true
     * @return false
     */
    virtual bool      setState(State state)                          = 0;
    virtual Direction getDirection() const                           = 0;
    virtual Event     waitForEvent(std::chrono::nanoseconds timeout) = 0;

protected:
    using IHalObject::IHalObject;
};  // namespace sugo::hal

}  // namespace sugo::hal

// Helper functions for type to string convertion
namespace std
{
/**
 * @brief std::ostream type presentation helper function
 */
inline std::ostream& operator<<(std::ostream& ostr, sugo::hal::IGpioPin::Direction const& pinDir)
{
    ostr << (pinDir == sugo::hal::IGpioPin::Direction::In ? "In" : "Out");
    return ostr;
}

/**
 * @brief std::ostream type presentation helper function
 */
inline std::ostream& operator<<(std::ostream& ostr, sugo::hal::IGpioPin::State const& pinState)
{
    ostr << (pinState == sugo::hal::IGpioPin::State::High ? "High" : "Low");
    return ostr;
}

}  // namespace std
