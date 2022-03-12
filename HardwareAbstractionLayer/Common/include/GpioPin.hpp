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

namespace sugo::hal
{
/**
 * @brief Class represents a GPIO pin
 *
 */
class GpioPin : public IGpioPin
{
public:
    using IGpioPin::IGpioPin;

    bool init(const IConfiguration& configuration) override;

    State     getState() const override;
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
