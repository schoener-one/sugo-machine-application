///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   26.09.2022
 */
///////////////////////////////////////////////////////////////////////////////

#include "ProcessContext.hpp"
#include "Logger.hpp"

using namespace sugo::message;

ProcessContext::ProcessContext(const std::string& instanceId, Thread::Policy policy, int priority)
    : m_thread(instanceId), m_policy(policy), m_priority(priority)
{
}

void ProcessContext::waitUntilFinished()
{
    if (m_thread.isRunning())
    {
        m_thread.join();
    }
}

bool ProcessContext::start()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_process)
    {
        LOG(error) << "Failed to start process, due to missing process instance";
        return false;
    }
    LOG(trace) << "Starting new process";
    return m_thread.start(m_process, m_policy, m_priority);
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