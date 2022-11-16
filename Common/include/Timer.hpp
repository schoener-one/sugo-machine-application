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
 * @brief Class represents a generic timer.
 *
 * @tparam TimePeriodT Time period type. Has to be compatible to std::chrono types.
 */
template <typename TimePeriodT = std::chrono::milliseconds>
class GenericTimer : public IRunnable
{
public:
    /// @brief Timout handler callback
    using TimeoutHandler = std::function<void()>;
    /// @brief Default timer clock
    using Clock = std::chrono::high_resolution_clock;

    /**
     * @brief Construct a new timer instance
     *
     * @param timeoutHandler Handler to be called in case of a timeout.
     * @param id             Id of this timer.
     */
    GenericTimer(const TimePeriodT& period, TimeoutHandler timeoutHandler, const std::string& id)
        : m_period(period), m_thread(id), m_timeoutHandler(std::move(timeoutHandler))
    {
    }

    /**
     * @brief Stops the timer and destroys the object
     *
     */
    ~GenericTimer() override
    {
        doStop();
    }

    bool start() override
    {
        m_nextWakeUpTime = Clock::now() + m_period;
        m_doRun          = true;

        std::unique_lock<std::mutex> lock(m_mutex);
        return m_thread.start([this] {
            while (m_doRun)
            {
                const auto now = Clock::now();

                if (this->m_nextWakeUpTime < now)
                {
                    // resync next wake up time!
                    const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - this->m_nextWakeUpTime);
                    const size_t periods = diff / this->m_period + 1;
                    this->m_nextWakeUpTime += periods * this->m_period;
                }

                assert(this->m_nextWakeUpTime >= now);

                {
                    std::unique_lock<std::mutex> lock(this->m_mutex);
                    m_condVariable.wait_until(lock, this->m_nextWakeUpTime);
                }

                this->m_nextWakeUpTime += this->m_period;

                if (m_doRun)
                {
                    this->m_timeoutHandler();
                }
            }
        });
    }

    bool isRunning() const override
    {
        return m_thread.isRunning();
    }

    void stop() override
    {
        doStop();
    }

private:
    void doStop()
    {
        if (m_thread.isRunning())
        {
            m_doRun = false;
            m_condVariable.notify_one();
            m_thread.join();
        }
    }

    const TimePeriodT       m_period;
    Clock::time_point       m_nextWakeUpTime;
    Thread                  m_thread;
    std::atomic_bool        m_doRun = false;
    TimeoutHandler          m_timeoutHandler;
    std::mutex              m_mutex;
    std::condition_variable m_condVariable;
};

/// @brief Class represents a timer.
using Timer = GenericTimer<>;
}  // namespace sugo
