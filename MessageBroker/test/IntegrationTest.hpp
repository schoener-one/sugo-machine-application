///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis
 * @date:   18.10.2022
 */
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

#include "Logger.hpp"
#include "Message.hpp"

class IntegrationTest : public ::testing::Test
{
protected:
    virtual ~IntegrationTest() = default;

    static void SetUpTestCase()
    {
        sugo::Logger::init(sugo::Logger::Severity::debug);
    }

    virtual void notifyReceivedMessage(const sugo::message::MessageId& message)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_messageReceiveQueue.push(message);
        lock.unlock();
        m_condVar.notify_one();
        LOG(debug) << "Received message: " << m_messageReceiveQueue.front();
    }

    void waitForReceivedMessages(
        const size_t                     expectedReceiveCount,
        const std::chrono::milliseconds& maxWaitTime = std::chrono::milliseconds(30000))
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cv_status               status = std::cv_status::no_timeout;

        while ((m_messageReceiveQueue.size() < expectedReceiveCount) &&
               (status == std::cv_status::no_timeout))
        {
            status = m_condVar.wait_for(lock, maxWaitTime);
        }

        ASSERT_EQ(status, std::cv_status::no_timeout);
    }

    std::queue<sugo::message::MessageId> m_messageReceiveQueue;

private:
    std::mutex                           m_mutex;
    std::condition_variable              m_condVar;
};
