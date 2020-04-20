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

#include "Thread.hpp"

namespace moco
{
/**
 * Class representing IO context.
 */
class IOContext : public boost::asio::io_service
{
public:
    /**
     * Starts the IO context thread.
     * @param policy Thread policy.
     * @param priority Thread priority.
     * @return true if the IO context could be started.
     */
    bool start(Thread::Policy policy = Thread::PolicyCurrent, int priority = 0);

    /**
     * Waits until IO context thread has finished.
     */
    void waitUntilFinish() { m_thread.join(); }

    Thread& getThread() { return m_thread; }

private:
    Thread m_thread;
};

}  // namespace moco

#endif  // IOCONTEXT_HPP_
