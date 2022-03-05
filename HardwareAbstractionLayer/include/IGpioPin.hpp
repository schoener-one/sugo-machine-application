///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-04
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <ostream>

#include "IHalObject.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for GPIO pins
 *
 */
class IGpioPin : virtual public IHalObject
{
public:
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

    enum Event
    {
        TimeoutEvent,  ///< Timeout event
        RisingEdge,    ///< Rising edge event
        FallingEdge    ///< Falling edge event
    };

    virtual ~IGpioPin()
    {
    }

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
    virtual bool      setState(State state)             = 0;
    virtual Direction getDirection() const              = 0;
    virtual bool      setDirection(Direction direction) = 0;
    virtual Event     waitForEvent(
            std::chrono::microseconds timeout = std::chrono::microseconds(0)) = 0;

protected:
    IGpioPin()
    {
    }
};

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