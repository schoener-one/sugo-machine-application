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

#include "HardwareAbstractionLayer/GpioPin.hpp"
#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/Simulator.hpp"

using namespace sugo::hal;

GpioPin::~GpioPin()
{
    finalize();
}

bool GpioPin::init(const common::IConfiguration& configuration)
{
    m_direction =
        (configuration.getOption(id::Direction).get<std::string>() == "in" ? Direction::In
                                                                           : Direction::Out);
    LOG(debug) << getId() << ".pin: " << configuration.getOption("pin").get<unsigned>();
    LOG(debug) << getId() << ".direction: " << m_direction;
    LOG(debug) << getId()
               << ".active-high: " << configuration.getOption(id::ActiveHigh).get<bool>();

    return true;
}

void GpioPin::finalize()
{
}

IGpioPin::State GpioPin::getState() const
{
    return Simulator::getInstance().getState(getId());
}

bool GpioPin::setState(GpioPin::State state)
{
    return Simulator::getInstance().setState(getId(), state);
}

IGpioPin::Direction GpioPin::getDirection() const
{
    return m_direction;
}

IGpioPin::Event GpioPin::waitForEvent(std::chrono::nanoseconds timeout)
{
    return Event{timeout, EventType::Timeout};
}
