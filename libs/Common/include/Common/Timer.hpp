///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2023
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

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>

#include "Common/IRunnable.hpp"
#include "Common/Thread.hpp"

namespace sugo::common
{
/**
 * @brief Class represents a generic timer.
 *
 * @tparam TimePeriodT Time period type. Has to be compatible to std::chrono types.
 * @tparam ClockT      Used chrono clock.
 */
template <typename TimePeriodT = std::chrono::milliseconds,
          typename ClockT      = std::chrono::high_resolution_clock>
class GenericTimer : public IRunnable
{
public:
    /// @brief Timout handler callback.
    using TimeoutHandler = std::function<void()>;

    /// @brief Used clock type.
    using Clock = ClockT;

    /**
     * @brief Construct a new timer instance
     *
     * @param timeoutHandler Handler to be called in case of a timeout. The handler will be called
     * within a dedicated thread context.
     * @param id             Id of this timer.
     */
    //
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

    /**
     * @brief Starts the timer within a new thread context.
     *
     * @return true if the timer could be started successfully.
     * @return false if the timer could not be started successfully.
     */
    bool start() override
    {
        assert(!isRunning());
        m_nextWakeUpTime = ClockT::now() + m_period;
        m_doRun          = true;

        std::unique_lock<std::mutex> lock(m_mutex);
        return m_thread.start([this] {
            while (m_doRun)
            {
                const auto now = ClockT::now();

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

    /**
     * @brief Indicates if the timer is running or not.
     *
     * @return true If the timer is running.
     * @return false If the timer is not running.
     */
    bool isRunning() const override
    {
        return m_thread.isRunning();
    }

    /**
     * @brief Stops the timer if it is running, otherwise does nothing.
     */
    void stop() override
    {
        doStop();
    }

private:
    /**
     * @brief Processes the timer stop in regard to the timer state.
     *
     */
    void doStop()
    {
        if (m_thread.isRunning())
        {
            m_doRun = false;
            m_condVariable.notify_one();
            m_thread.join();
        }
    }

    const TimePeriodT          m_period;          ///< Time period of the timer.
    typename Clock::time_point m_nextWakeUpTime;  ///< Calculated next timeout time.
    Thread                     m_thread;          ///< Timer thread.
    std::atomic_bool           m_doRun{false};    ///< Internal timer status indication.
    TimeoutHandler             m_timeoutHandler;  ///< Timeout handler to be called.
    std::mutex                 m_mutex;           ///< Mutex object.
    std::condition_variable    m_condVariable;    ///< Conditional variable to wait on.
};

/// @brief Class represents a timer.
using Timer = GenericTimer<>;
}  // namespace sugo::common
