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

using namespace sugo;

Thread::Thread(const std::string& id) : m_id(id), m_isReady(false), m_policy(PolicyCurrent)
{
}

bool Thread::start(Runnable function, Policy policy, int priority)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_isReady  = false;
    m_runnable = function;
    m_policy   = policy;

    m_thread = std::thread([&] {
        Logger::reinit(getId());
        bool success = true;
        if (m_policy == PolicyRealTime)
        {
            success = prepareRealTimeContext();
        }

        if (success)
        {
            // wait until thread is prepared for real-time context!
            std::unique_lock<std::mutex> lockThread(m_mutex);
            m_condVar.wait(lockThread, [&] { return m_isReady; });
            // now start...
            LOG(debug) << "Starting new " << ((m_policy == PolicyRealTime) ? "real-time " : "")
                       << "thread: " << std::hex << std::setw(8) << std::setfill('0')
                       << std::this_thread::get_id();
            try
            {
                m_runnable();
            }
            catch (const std::exception& ex)
            {
                LOG(error) << "Exception catched: " << ex.what();
            }
            catch (...)
            {
                LOG(error) << "Exception catched";
            }
        }
        else
        {
            LOG(error) << "Failed to prepare real-time context";
        }
        m_isReady = false;
    });
    assert(m_thread.joinable() == true);

    if (policy == PolicyRealTime)
    {
        m_isReady = setRealTimePolicy(priority);
    }
    else
    {
        m_isReady = true;
    }

    // let the prepared thread run...
    m_condVar.notify_one();
    return m_isReady;
}

bool Thread::prepareRealTimeContext()
{
    bool success = (::mlockall(MCL_CURRENT | MCL_FUTURE) == 0);
    return success;
}

bool Thread::setRealTimePolicy(int priority)
{
    sched_param schedParam;
    int         policy = 0;
    bool success       = pthread_getschedparam(m_thread.native_handle(), &policy, &schedParam) == 0;

    if (success)
    {
        LOG(debug) << "Creating new real-time thread " << getId() << " with priority " << priority;

        schedParam.sched_priority = priority;
        success = pthread_setschedparam(m_thread.native_handle(), SCHED_FIFO, &schedParam) == 0;

        if (!success)
        {
            LOG(error) << "Failed to set scheduler parameters for thread " << std::hex << getId();
        }
    }
    else
    {
        LOG(error) << "Failed to get scheduler parameters for thread " << getId();
    }
    return success;
}
