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
    constexpr static int DefaultPriority = 0;

    explicit Thread(int priority = DefaultPriority);

    using Runnable = std::function<void(void)>;
    void            start(Runnable function);
    void            join() { m_thread.join(); }
    std::thread::id getId() { return m_thread.get_id(); }

    /**
     * Prepares the real-time thread context. Should be called from within
     * every real-time thread.
     * @return true if succeeded.
     */
    static bool prepareRealTimeContext();

private:
    std::mutex              m_mutex;
    std::condition_variable m_condVar;
    bool                    m_isReady;
    std::thread             m_thread;
    const int               m_priority;
};
}  // namespace moco

#endif /* THREAD_HPP_ */
