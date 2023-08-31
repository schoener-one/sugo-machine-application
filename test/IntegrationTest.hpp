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
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "Common/Logger.hpp"
#include "MessageBroker/MessageBroker.hpp"
#include "ServiceComponent/ServiceId.hpp"

// #include <Command.pb.h>

/// Expect a defined state of that component
#define EXPECT_STATE(_COMP, _STATE) expectState<_COMP>(_COMP::State::_STATE)
#define EXPECT_NOTIFICATION_SUBSCRIBE(_COMP, _NOTIF) subscribeToNotification<_COMP>(_COMP::_NOTIF)
#define EXPECT_NOTIFICATION(_COMP, _NOTIF) waitForNotification<_COMP>(_COMP::_NOTIF)

namespace sugo
{
class IntegrationTest : public ::testing::Test
{
protected:
    explicit IntegrationTest(const message_broker::Address& address)
        : m_receiverAddress(address), m_ioContext(address), m_broker(address, m_ioContext)
    {
    }
    virtual ~IntegrationTest() = default;

    void SetUp() override
    {
        m_receivedNotifications.clear();
        ASSERT_TRUE(m_broker.start());
    }

    void TearDown() override
    {
        m_broker.stop();
    }

    message_broker::ResponseMessage send(const service_component::RequestId& requestId)
    {
        message_broker::Message message;
        message.setId(requestId.getMessageId());
        message.setSequence(m_idCount);
        message_broker::ResponseMessage response;
        EXPECT_TRUE(m_broker.send(message, requestId.getAddress(), response));
        EXPECT_EQ(response.getResult(), message_broker::ResponseMessage::Result::Success);
        EXPECT_EQ(response.getSequence(), m_idCount);
        m_idCount++;
        return response;
    }

    template <typename ComponentT>
    void expectState(const typename ComponentT::State state)
    {
        LOG(info) << "!! Expect state: " << state;
        std::unique_lock<std::mutex> lock(m_mutex);
        auto                         response = send(ComponentT::RequestGetState);
        const common::Json& jsonState = common::Json::parse(response.getPayload()).at("state");
        EXPECT_FALSE(jsonState.empty());
        EXPECT_EQ(static_cast<unsigned>(state), jsonState.get<unsigned>());
    }

    template <typename ComponentT>
    void subscribeToNotification(const service_component::NotificationId& notificationId)
    {
        m_broker.registerNotificationMessageHandler(
            notificationId.getMessageId(), [&](const message_broker::Message& message) {
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_receivedNotifications.push_back(message.getId());
                }
                m_condVar.notify_one();
            });
        ASSERT_TRUE(m_broker.subscribe(notificationId.getAddress(), notificationId.getTopic()));
        LOG(debug) << "Subscribed for notification: " << notificationId;
    }

    template <typename ComponentT>
    void waitForNotification(
        const service_component::NotificationId& notificationId,
        const std::chrono::milliseconds&         maxTime = std::chrono::milliseconds(60000))
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cv_status               status = std::cv_status::no_timeout;

        LOG(info) << "!! Expect notification: " << notificationId;

        while ((status == std::cv_status::no_timeout) &&
               (!hasNotificationReceived(notificationId.getMessageId())))
        {
            status = m_condVar.wait_for(lock, maxTime);
            EXPECT_NE(std::cv_status::timeout, status);
        }
        ASSERT_TRUE(m_broker.unsubscribe(notificationId.getAddress(), notificationId.getTopic()));
        m_broker.unregisterMessageHandler(notificationId.getMessageId());
        LOG(info) << ">> Expected notification received: " << notificationId;
    }

private:
    bool hasNotificationReceived(const message_broker::Message::Identifier& id)
    {
        auto it = std::find(m_receivedNotifications.cbegin(), m_receivedNotifications.cend(), id);

        if (it == m_receivedNotifications.cend())
        {
            return false;
        }

        m_receivedNotifications.erase(it);
        return true;
    }

    message_broker::Address       m_receiverAddress;
    common::IOContext             m_ioContext;
    message_broker::MessageBroker m_broker;

    unsigned                                         m_idCount = 0;
    std::vector<message_broker::Message::Identifier> m_receivedNotifications;
    std::mutex                                       m_mutex;
    std::condition_variable                          m_condVar;
};

}  // namespace sugo