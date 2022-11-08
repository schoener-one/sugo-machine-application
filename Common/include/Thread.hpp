/*
 * Thread.hpp
 *
 *  Created on: 08.12.2019
 *      Author: denis
 */

#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

namespace sugo
{
/**
 * Thread class for a real-time context.
 */
class Thread final
{
public:
    /// Default priority
    enum Policy
    {
        PolicyCurrent,  ///< Keep current thread policy type.
        PolicyRealTime  ///< Change to real time thread policy.
    };

    /// @brief The default thread policy
    static constexpr Thread::Policy DefaultPolicy = Thread::PolicyCurrent;
    /// @brief The default thread priority
    static constexpr int DefaultPriority = 0;

    /**
     * @brief Construct a new thread object.
     *
     * @param id Identifier of this thread
     */
    explicit Thread(const std::string& id);
    ~Thread() = default;

    /// @brief Runnable function type.
    using Runnable = std::function<void()>;

    /**
     * Starts a new thread with the runnable function.
     *
     * @param function Function to be executed within the thread context.
     * @return true if the function could be started.
     */
    bool start(Runnable function, Policy policy = DefaultPolicy, int priority = DefaultPriority);

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

    std::string             m_id;
    std::mutex              m_mutex;
    std::condition_variable m_condVar;
    bool                    m_isReady;
    std::thread             m_thread;
    Runnable                m_runnable = nullptr;
    Policy                  m_policy;
};
}  // namespace sugo

#endif /* THREAD_HPP_ */
