///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-14
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
    Event     waitForEvent(std::chrono::nanoseconds timeout = std::chrono::nanoseconds(0)) override;

private:
    Direction      m_direction = Direction::In;
    GpioPin::State m_state     = GpioPin::State::Low;
};

}  // namespace sugo::hal
