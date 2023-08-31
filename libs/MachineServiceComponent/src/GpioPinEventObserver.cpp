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

#include <cassert>

#include "MachineServiceComponent/GpioPinEventObserver.hpp"

using namespace sugo;
using namespace sugo::machine_service_component;

GpioPinEventObserver::GpioPinEventObserver(
    std::shared_ptr<hal::IGpioPin> pin, EventHandler eventHandler,
    const std::chrono::milliseconds& maxGpioPinObservationTimeout)
    : m_pin(std::move(pin)),
      m_eventHandler(std::move(eventHandler)),
      m_maxGpioPinObservationTimeout(maxGpioPinObservationTimeout),
      m_thread(m_pin->getId() + "Observer")
{
}

GpioPinEventObserver::~GpioPinEventObserver()
{
    doStop();
}

bool GpioPinEventObserver::start()
{
    assert(!m_thread.isRunning());
    assert(m_pin);
    assert(m_eventHandler);
    m_doObserve = true;

    return m_thread.start([this] {
        while (m_doObserve)
        {
            const auto event =
                this->m_pin->waitForEvent(std::chrono::duration_cast<std::chrono::nanoseconds>(
                    this->m_maxGpioPinObservationTimeout));

            if (m_doObserve && (event.type != hal::IGpioPin::EventType::Timeout))
            {
                this->m_eventHandler(event, this->getId());
            }
        }
    });
}

void GpioPinEventObserver::stop()
{
    doStop();
}

void GpioPinEventObserver::doStop()
{
    if (m_thread.isRunning())
    {
        m_doObserve = false;
        m_thread.join();
    }
}
