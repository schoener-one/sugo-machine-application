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

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <mutex>
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
    /// @brief Default timer clock
    using clock = std::chrono::high_resolution_clock;

    /**
     * @brief Construct a new timer instance
     *
     * @param timeoutHandler Handler to be called in case of a timeout.
     * @param id             Id of this timer.
     */
    Timer(TimeoutHandler timeoutHandler, const std::string& id)
        : m_thread(id), m_timeoutHandler(timeoutHandler)
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
        m_nextWakeUpTime = clock::now() + period;
        m_doRun          = true;

        std::unique_lock<std::mutex> lock(m_mutex);
        return m_thread.start([this, &period] {
            while (m_doRun)
            {
                const auto now = clock::now();

                if (this->m_nextWakeUpTime < now)
                {
                    // resync next wake up time!
                    const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - this->m_nextWakeUpTime);
                    const size_t periods = diff / period + 1;
                    this->m_nextWakeUpTime += periods * period;
                }

                assert(this->m_nextWakeUpTime >= now);

                {
                    std::unique_lock<std::mutex> lock(this->m_mutex);
                    m_condVariable.wait_until(lock, this->m_nextWakeUpTime);
                }

                this->m_nextWakeUpTime += period;

                if (m_doRun)
                {
                    this->m_timeoutHandler();
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
        m_doRun = false;
        m_condVariable.notify_one();
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_thread.isRunning())
        {
            lock.unlock();
            m_thread.join();
        }
    }

private:
    clock::time_point       m_nextWakeUpTime;
    Thread                  m_thread;
    std::atomic_bool        m_doRun = false;
    TimeoutHandler          m_timeoutHandler;
    std::mutex              m_mutex;
    std::condition_variable m_condVariable;
};
}  // namespace sugo
