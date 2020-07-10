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
#include <thread>

namespace moco
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

    explicit Thread();

    /// Runnable function
    using Runnable = std::function<void(void)>;

    /**
     * Starts a new thread with the runnable function.
     *
     * @param function Function to be executed within the thread context.
     * @return true if the function could be started.
     */
    bool start(Runnable function, Policy policy = PolicyCurrent, int priority = 0);

    void join() { m_thread.join(); }

    std::thread::id getId() { return m_thread.get_id(); }

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

    std::mutex              m_mutex;
    std::condition_variable m_condVar;
    bool                    m_isReady;
    std::thread             m_thread;
    Runnable                m_runnable;
    Policy                  m_policy;
};
}  // namespace moco

#endif /* THREAD_HPP_ */
