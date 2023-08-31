///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
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

#include <gtest/gtest.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

#include "Common/Logger.hpp"
#include "MessageBroker/Message.hpp"

using namespace sugo::message_broker;
using namespace sugo;

class IntegrationTest : public ::testing::Test
{
protected:
    virtual ~IntegrationTest() = default;

    static void SetUpTestCase()
    {
        sugo::common::Logger::init(sugo::common::Logger::Severity::debug);
    }

    virtual void notifyReceivedMessage(const Message::Identifier& messageId)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_messageReceiveQueue.push(messageId);
        lock.unlock();
        m_condVar.notify_one();
        LOG(debug) << "Received message id: " << m_messageReceiveQueue.back();
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

    std::queue<Message::Identifier> m_messageReceiveQueue;

private:
    std::mutex              m_mutex;
    std::condition_variable m_condVar;
};
