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

#include "Common/IRunnable.hpp"
#include "Common/Thread.hpp"
#include "HardwareAbstractionLayer/IGpioPin.hpp"

#include <chrono>
#include <functional>
#include <memory>

namespace sugo::machine_service_component
{
/// @brief Class to observe GPIO events.
class GpioPinEventObserver : common::IRunnable
{
public:
    /// @brief Event handler which receives the GPIO pin events.
    using EventHandler = std::function<void(const hal::IGpioPin::Event&, const hal::Identifier&)>;

    /**
     * @brief Construct a new Gpio pin event observer.
     *
     * @param pin          GPIO pin to observe.
     * @param eventHandler Event handler to be called in case of an event.
     */
    GpioPinEventObserver(std::shared_ptr<hal::IGpioPin> pin, EventHandler eventHandler,
                         const std::chrono::milliseconds& maxGpioPinObservationTimeout);
    ~GpioPinEventObserver() override;

    bool start() override;
    void stop() override;

    bool isRunning() const override
    {
        return m_thread.isRunning();
    }

    /**
     * @brief Returns the pin id.
     *
     * @return The pin id.
     */
    const hal::Identifier& getId() const
    {
        return m_pin->getId();
    }

private:
    /// @brief Stops the observation.
    void doStop();

    std::shared_ptr<hal::IGpioPin> m_pin;           ///< Pin object to observe.
    EventHandler                   m_eventHandler;  ///< Event handler instance.
    const std::chrono::milliseconds
                   m_maxGpioPinObservationTimeout;  ///< Max pin observation timeout.
    common::Thread m_thread;                        ///< Observation thread instance.
    bool           m_doObserve = false;  ///< Indicates if the observation should be continued.
};
}  // namespace sugo::machine_service_component