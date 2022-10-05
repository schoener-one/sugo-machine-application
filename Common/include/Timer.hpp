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

#include "IRunnable.hpp"
#include "Thread.hpp"

#include <chrono>
#include <functional>
#include <string>

namespace sugo
{
/**
 * @brief Class to provide a timer functionality
 *
 */
class Timer
{
public:
    /// @brief Timout handler callback
    using TimeoutHandler = std::function<void()>;

    /**
     * @brief Construct a new timer instance
     *
     * @param timeoutHandler Handler to be called in case of a timeout.
     * @param name           Name of this timer.
     */
    Timer(TimeoutHandler timeoutHandler, const std::string& name = "Timer")
        : m_thread(name), m_timeoutHandler(timeoutHandler)
    {
    }
    /**
     * @brief Stops the timer and destroys the object
     *
     */
    ~Timer()
    {
        stop();
    }

    /**
     * @brief Starts the timer.
     *
     * @param duration Time to wait before calling the timeout handler.
     * @return true  If the start succeeded and the timer could be started.
     * @return false If the start failed.
     */
    template <typename RepT, typename PeriodT>
    bool start(const std::chrono::duration<RepT, PeriodT>& period)
    {
        m_isRunning = true;
        return m_thread.start([&] {
            for (; m_isRunning;)
            {
                std::this_thread::sleep_for(period);
                if (m_isRunning)
                {
                    m_timeoutHandler();
                }
            }
        });
    }

    /**
     * @brief Checks if the timer is still running.
     *
     * @return true  If the timer is running.
     * @return false If the timer is not running.
     */
    bool isRunning()
    {
        return m_thread.isRunning();
    }

    /**
     * @brief Stop the running timer and waits for the thread end,
     * which could be until one period.
     *
     */
    void stop()
    {
        if (m_isRunning)
        {
            m_isRunning = false;
            m_thread.join();
        }
    }

private:
    Thread         m_thread;
    bool           m_isRunning = false;
    TimeoutHandler m_timeoutHandler;
};
}  // namespace sugo