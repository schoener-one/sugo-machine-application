///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IGpioPin.hpp"
#include "IRunnable.hpp"
#include "Thread.hpp"

#include <chrono>
#include <functional>
#include <memory>

namespace sugo
{
/**
 * @brief Class to observe GPIO events.
 *
 */
class GpioPinEventObserver : IRunnable
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
    GpioPinEventObserver(const std::shared_ptr<hal::IGpioPin> pin, EventHandler eventHandler,
                         const std::chrono::milliseconds& maxGpioPinObservationTimeout);
    virtual ~GpioPinEventObserver();

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
    void doStop();

    std::shared_ptr<hal::IGpioPin>  m_pin;
    EventHandler                    m_eventHandler;
    const std::chrono::milliseconds m_maxGpioPinObservationTimeout;
    Thread                          m_thread;
    bool                            m_doObserve = false;
};
}  // namespace sugo