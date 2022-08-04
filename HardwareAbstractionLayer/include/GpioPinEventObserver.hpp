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

#include <functional>
#include <memory>

namespace sugo::hal
{
/**
 * @brief Class to observe GPIO events in thread context
 *
 */
class GpioPinEventObserver
{
public:
    using Handler = std::function<void(const IGpioPin::Event&)>;

    /**
     * @brief Starts the GPIO pin observation.
     *
     * @param handler Handler which receives the event.
     * @return true  if the observer could be started.
     * @return false if the observer could no be started.
     */
    bool start(const std::shared_ptr<IGpioPin> pin, Handler handler)
    {
        m_pin    = pin;
        m_doWait = true;
        return m_thread.start([&] {
            assert(m_pin);
            IGpioPin::Event event;
            do
            {
                event = m_pin->waitForEvent(std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::milliseconds(100)));
            } while (m_doWait && (event.type == IGpioPin::EventType::TimeoutEvent));

            if (event.type != IGpioPin::EventType::TimeoutEvent)
            {
                handler(event);
            }
        });
    }

    /**
     * @brief Checks if the observer is still running.
     *
     * @return true  if the observer is running.
     * @return false if the observer is not running.
     */
    bool isRunning()
    {
        return m_thread.isRunning();
    }

    /**
     * @brief Stop the running observer and waits for the thread end.
     *
     */
    void stop()
    {
        if (isRunning())
        {
            m_doWait = false;
            m_thread.join();
            m_pin.reset();
        }
    }

private:
    std::shared_ptr<IGpioPin> m_pin;
    Thread                    m_thread;
    bool                      m_doWait = false;
};
}  // namespace sugo::hal