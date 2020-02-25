/*
 * Thread.cpp
 *
 *  Created on: 08.12.2019
 *      Author: denis
 */
#include <pthread.h>
#include <sched.h>
#include <cassert>

#include <sys/mman.h>
#include <Thread.hpp>

#include "Globals.hpp"

using namespace moco;

Thread::Thread(int priority) : m_isReady(false), m_thread(), m_priority(priority) {}

void Thread::start(Runnable function)
{
    m_isReady = false;
    std::unique_lock<std::mutex> lock(m_mutex);

    m_thread = std::thread([&] {
        Logger::reinit();
        if (prepareRealTimeContext())
        {
            // wait until thread is prepared for real-time context!
            std::unique_lock<std::mutex> lockThread(m_mutex);
            m_condVar.wait(lockThread, [&] { return m_isReady; });
            // now start...
            LOG(debug) << "Starting new real-time thread: " << std::this_thread::get_id();
            function();
        }
        else
        {
            LOG(error) << "Failed to prepare real-time context";
        }
    });
    assert(m_thread.joinable() == true);
    sched_param schedParam;
    int         policy = 0;
    pthread_getschedparam(m_thread.native_handle(), &policy, &schedParam);

    policy = SCHED_FIFO;

    int priority = m_priority;
    if (priority == DefaultPriority)
    {
        priority = (sched_get_priority_max(policy) - sched_get_priority_min(policy)) / 2;
    }
    LOG(debug) << "Creating new real-time thread " << getId() << " with priority " << priority;

    schedParam.sched_priority = priority;
    const int rv = pthread_setschedparam(m_thread.native_handle(), SCHED_FIFO, &schedParam);
    assert(rv == 0);

    // let the prepared thread run...
    m_isReady = true;
    m_condVar.notify_one();
}

bool Thread::prepareRealTimeContext()
{
    bool success = (::mlockall(MCL_CURRENT | MCL_FUTURE) == 0);
    return success;
}
