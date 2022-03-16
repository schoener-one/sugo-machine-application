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

#include <gpiod.hpp>

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
    GpioPin(const Identifier& id, gpiod::chip chip) : IGpioPin(id), m_chip(chip)
    {
    }

    ~GpioPin() override;

    bool init(const IConfiguration& configuration) override;

    void finalize();

    State     getState() const override;
    bool      setState(State state) override;
    Direction getDirection() const override;
    Event     waitForEvent(std::chrono::nanoseconds timeout = std::chrono::nanoseconds(0)) override;

private:
    gpiod::chip m_chip;
    gpiod::line m_line;
    Direction   m_direction = Direction::In;
};

}  // namespace sugo::hal
