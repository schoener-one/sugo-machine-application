///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   08.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef IOCONTEXT_HPP_
#define IOCONTEXT_HPP_

#include <boost/asio.hpp>

#include "IRunnable.hpp"
#include "Thread.hpp"

namespace sugo
{
/**
 * Class representing IO context.
 */
class IOContext : public boost::asio::io_service, public IRunnable
{
public:
    static constexpr Thread::Policy DefaultPolicy   = Thread::PolicyCurrent;
    static constexpr int            DefaultPriority = 0;

    virtual ~IOContext();

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
    bool start() override;

    void stop() override;

    bool isRunning() const override { return m_thread.isRunning(); }
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

    Thread& getThread() { return m_thread; }

private:
    Thread         m_thread;
    Thread::Policy m_policy;
    int            m_priority;
};

}  // namespace sugo

#endif  // IOCONTEXT_HPP_
