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

#include <gpiod.hpp>

#include "HardwareAbstractionLayer/IGpioPin.hpp"

namespace sugo::hal
{
/// @brief Class represents a GPIO pin
class GpioPin : public IGpioPin
{
public:
    /**
     * @brief Construct a new Gpio pin.
     *
     * @param id Identifier of the GPIO chip.
     * @param chip Representing GPIO chip object.
     */
    GpioPin(const Identifier& id, gpiod::chip chip) : IGpioPin(id), m_chip(std::move(chip))
    {
    }
    /**
     * @brief Destructor of the GPIO pin.
     *
     */
    ~GpioPin() override;

    bool init(const common::IConfiguration& configuration) override;
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
