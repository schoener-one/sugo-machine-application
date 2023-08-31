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

#include "Common/ProcessContext.hpp"
#include "Common/Logger.hpp"

using namespace sugo::common;

void ProcessContext::waitUntilFinished()
{
    if (m_thread.isRunning())
    {
        m_thread.join();
    }
}

bool ProcessContext::start()
{
    assert(!isRunning());
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_process)
    {
        LOG(error) << "Failed to start process, due to missing process instance";
        return false;
    }
    LOG(trace) << "Starting new process";
    return m_thread.start(m_process);
}

void ProcessContext::stop()
{
    LOG(debug) << "Stopping process";
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stopProcess)
    {
        m_stopProcess();
    }
    else
    {
        LOG(warning) << "Missing stop process function";
    }
    waitUntilFinished();
    LOG(trace) << "Process stopped";
}

bool ProcessContext::setProcessRunner(Thread::Runnable process, Thread::Runnable stopProcess)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (isRunning())
    {
        LOG(error) << "Failed to set process instance, due to already running instance";
        return false;
    }
    m_process     = process;
    m_stopProcess = stopProcess;
    return true;
}