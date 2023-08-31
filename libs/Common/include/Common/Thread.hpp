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

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

namespace sugo::common
{
/**
 * @brief Class represents a thread which could be controlled by the object.
 * The represented thread could be processed with or without a real-time scheduling
 * depending on the policy type.
 */
class Thread final
{
public:
    /// @brief Runnable function type.
    using Runnable = std::function<void()>;

    /// @brief Priority type.
    using Priority = int;

    /// @brief Default priority
    enum Policy
    {
        PolicyCurrent,  ///< Keep current thread policy type.
        PolicyRealTime  ///< Change to real time thread policy.
    };

    /// @brief The default thread policy.
    static constexpr Policy DefaultPolicy = PolicyCurrent;

    /// @brief The default thread priority.
    static constexpr int DefaultPriority = 0;

    /**
     * @brief Constructs a new Thread object
     *
     * @param id Identifier of this thread.
     * @param policy Thread policy.
     * @param priority Thread priority.
     */
    explicit Thread(std::string id, Policy policy = DefaultPolicy,
                    Priority priority = DefaultPriority);

    /// @brief Default destructor.
    ~Thread() = default;

    /// @brief Copy constructor
    Thread(const Thread&) = delete;

    /**
     * @brief Move constructor.
     *
     * @param thread Thread to move to this.
     * @warning This could cause unexpected behaviour, because not all members are movable, so use
     * it with care!
     */
    Thread(Thread&& thread);

    /// @brief Copy operator
    /// @return Thread
    Thread& operator=(const Thread&) = delete;

    /**
     * @brief Move operator.
     *
     * @param thread Thread to move to this.
     * @return This thread.
     * @warning This could cause unexpected behaviour, because not all members are movable, so use
     * it with care!
     */
    Thread& operator=(Thread&& thread);

    /**
     * Starts a new thread with the runnable function.
     *
     * @param function Function to be executed within the thread context.
     * @return true if the function could be started.
     */
    bool start(Runnable function);

    /**
     * Indicates if thread is still running.
     * @return true if thread is still running.
     */
    bool isRunning() const
    {
        return m_thread.joinable();
    }

    /**
     * @brief Waits for the thread to be finished.
     *
     */
    void join()
    {
        m_thread.join();
    }

    /**
     * @brief Get the identifier of this thread.
     *
     * @return Thread identifier.
     */
    const std::string& getId()
    {
        return m_id;
    }

    /**
     * @brief Returns the thread policy.
     *
     * @return The policy
     */
    Policy getPolicy() const
    {
        return m_policy;
    }

    /**
     * @brief Returns the thread priority.
     *
     * @return The priority.
     */
    Priority getPriority() const
    {
        return m_priority;
    }

private:
    /**
     * Prepares the real-time thread context. Should be called from within
     * every real-time thread.
     * @return true if succeeded.
     */
    static bool prepareRealTimeContext();

    /**
     * Switches the thread to real time policy.
     * @param priority Priority to be set for real time thread.
     * @return
     */
    bool setRealTimePolicy(int priority);

    std::string             m_id;                  ///< Unique thread identifier.
    std::mutex              m_mutex;               ///< Mutex object.
    std::condition_variable m_condVar;             ///< Condition variable for thread controlling.
    bool                    m_isReady = false;     ///< Indicates if thread is ready to run.
    std::thread             m_thread;              ///< Thread object handle.
    Runnable                m_runnable = nullptr;  ///< The runnable function object.
    Policy                  m_policy   = Policy::PolicyCurrent;  ///< Associated thread policy.
    Priority                m_priority = DefaultPriority;        ///< Associated thread priority.
};
}  // namespace sugo::common
