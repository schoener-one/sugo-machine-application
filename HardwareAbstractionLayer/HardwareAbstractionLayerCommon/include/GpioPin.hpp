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

#include "IGpioPin.hpp"
#include "HalObject.hpp"

namespace sugo::hal
{
/**
 * @brief Base class for GPIO pins
 *
 */
class GpioPin : public HalObject, public IGpioPin
{
public:
    constexpr static unsigned InvalidPin = 0xffffffff;

    GpioPin(unsigned pin) : m_pin(pin) {}
    virtual ~GpioPin() {}

    bool init(Direction direction, State state, bool activeHigh = true);

    /**
     * @brief Returns the state of the pin.
     *
     * @return State of the pin
     */
    State getState() const override;

    /**
     * @brief Set the State of the GPIO
     *
     * @param state
     * @return true
     * @return false
     */
    bool      setState(State state) override;
    Direction getDirection() const override;
    bool      setDirection(Direction direction) override;
    Event waitForEvent(std::chrono::microseconds timeout = std::chrono::microseconds(0)) override;

private:
    unsigned  m_pin        = InvalidPin;
    Direction m_direction  = Direction::In;
    bool      m_activeHigh = true;
};

}  // namespace sugo::hal
