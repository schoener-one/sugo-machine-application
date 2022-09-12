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
     * @brief Construct a new Timer object
     *
     * @param timeoutHandler
     */
    Timer(TimeoutHandler timeoutHandler) : m_thread("Timer"), m_timeoutHandler(timeoutHandler)
    {
    }
    ~Timer() = default;

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
                m_timeoutHandler();
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