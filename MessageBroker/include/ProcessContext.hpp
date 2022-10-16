///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   26.09.2022
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IProcessContext.hpp"
#include "Thread.hpp"

#include <functional>
#include <mutex>

namespace sugo::message
{
/**
 * Class representing the process context.
 */
class ProcessContext : public IProcessContext
{
public:
    /**
     * @brief Construct a new work context object
     *
     * @param instanceId Identifier for this instance.
     * @param policy     Thread policy to be used.
     * @param priority   Priority to be apply to the working instances.
     */
    ProcessContext(const std::string& instanceId, Thread::Policy policy = Thread::DefaultPolicy,
                   int priority = Thread::DefaultPriority);
    virtual ~ProcessContext();

    ProcessContext& operator=(const ProcessContext&) = delete;
    ProcessContext& operator=(ProcessContext&&) = delete;

    bool isRunning() const override
    {
        return m_thread.isRunning();
    }

    bool start() override;
    void stop() override;
    void waitUntilFinished() override;

    /**
     * @brief Set the process runner function to be used on start from this context.
     * Must not be set during running!
     *
     * @param process Process to be used.
     * @return true  If the process has not yet started and could be set.
     * @return false If the process has already started.
     */
    bool setProcessRunner(Thread::Runnable process,
                          Thread::Runnable stopProcess = nullptr) override;

private:
    Thread           m_thread;                 ///< Main worker thread
    Thread::Policy   m_policy;                 ///< Thread policy
    int              m_priority;               ///< Thread priority
    Thread::Runnable m_process     = nullptr;  ///< Process function to run
    Thread::Runnable m_stopProcess = nullptr;  ///< Function to stop a running process
    std::mutex       m_mutex;                  ///< Mutex
};

}  // namespace sugo
