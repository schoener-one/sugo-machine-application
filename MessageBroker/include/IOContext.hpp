///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   08.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRunnable.hpp"
#include "Thread.hpp"

namespace sugo
{
/**
 * Class representing IO context.
 */
class IOContext : public IRunnable
{
public:
    static constexpr Thread::Policy DefaultPolicy   = Thread::PolicyCurrent;
    static constexpr int            DefaultPriority = 0;

    IOContext(Thread::Policy policy = DefaultPolicy, int priority = DefaultPriority)
        : m_policy(policy), m_priority(priority)
    {
    }

    /**
     * Sets a new policy. The policy will only be applied on next start.
     * @param policy   New thread policy.
     * @param priority New thread priority.
     */
    void setPolicy(Thread::Policy policy, int priority)
    {
        m_policy   = policy;
        m_priority = priority;
    }

    // IRunnable {{
    bool isRunning() const override
    {
        return m_thread.isRunning();
    }
    // IRunnable }}

    /**
     * Waits until IO context thread has finished.
     */
    void waitUntilFinished()
    {
        if (m_thread.isRunning())
        {
            m_thread.join();
        }
    }

    Thread& getThread()
    {
        return m_thread;
    }

protected:
    Thread         m_thread;
    Thread::Policy m_policy;
    int            m_priority;
};

}  // namespace sugo
