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

#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <cassert>
#include <iomanip>

#include "Common/Ios.hpp"
#include "Common/Thread.hpp"
#include "Common/Types.hpp"

namespace
{
constexpr std::size_t MaxThreadNameSize = 15;  // + \0 = 16!
}

using namespace sugo::common;

Thread::Thread(std::string id, Thread::Policy policy, Priority priority)
    : m_id(std::move(id)), m_policy(policy), m_priority(priority)
{
}

Thread::Thread(Thread&& thread)
    : m_id(std::move(thread.m_id)),
      m_isReady(false),
      m_runnable(std::move(thread.m_runnable)),
      m_policy(std::move(thread.m_policy)),
      m_priority(std::move(thread.m_priority))
{
}

Thread& Thread::operator=(Thread&& thread)
{
    *this = std::move(thread);
    return *this;
}

bool Thread::start(Runnable function)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_isReady  = false;
    m_runnable = std::move(function);

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
            LOG(trace) << "Starting new " << ((m_policy == PolicyRealTime) ? "real-time " : "")
                       << "thread: " << sugo::common::ios::hex<>(std::this_thread::get_id());
            try
            {
                m_runnable();
            }
            catch (const std::exception& ex)
            {
                LOG(fatal) << "Exception catched: " << ex.what();
            }
            catch (...)
            {
                LOG(fatal) << "Exception catched";
            }
        }
        else
        {
            LOG(error) << "Failed to prepare real-time context";
        }
        m_isReady = false;
    });
    // Set thread visible name
    const std::string shortName(
        m_id.substr(0, (m_id.size() > MaxThreadNameSize) ? MaxThreadNameSize : m_id.size()));
    assert(pthread_setname_np(m_thread.native_handle(), shortName.c_str()) == 0);

    if (m_policy == PolicyRealTime)
    {
        m_isReady = setRealTimePolicy(m_priority);
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
    sched_param schedParam{0};
    int         policy = 0;
    bool success = (pthread_getschedparam(m_thread.native_handle(), &policy, &schedParam) == 0);

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
