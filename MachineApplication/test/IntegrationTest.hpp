//////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-10
 */
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "CommandMessageBroker.hpp"
#include "Logger.hpp"
#include "ServiceComponent.hpp"

#include <Command.pb.h>

/// Expect a defined state of that component
#define EXPECT_STATE(_COMP, _STATE) expectState<_COMP>(_COMP::State::_STATE)
#define EXPECT_NOTIFICATION_SUBSCRIBE(_COMP, _NOTIF) subscribeToNotification<_COMP>(_COMP::_NOTIF)
#define EXPECT_NOTIFICATION(_COMP, _NOTIF) waitForNotification<_COMP>(_COMP::_NOTIF)

namespace sugo
{
class IntegrationTest : public ::testing::Test
{
protected:
    explicit IntegrationTest(const message::Address& address)
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

    message::CommandResponse send(const message::CommandId& commandId)
    {
        message::Command command;
        command.set_id(m_idCount);
        command.set_name(commandId.getId());
        message::CommandResponse response;
        EXPECT_TRUE(m_broker.send(command, commandId.getReceiverAddress(), response));
        EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
        EXPECT_EQ(response.id(), m_idCount);
        m_idCount++;
        return response;
    }

    template <typename CompT>
    void expectState(const typename CompT::State state)
    {
        LOG(debug) << "Expect state: " << state;
        std::unique_lock<std::mutex> lock(m_mutex);
        auto                         response  = send(CompT::CommandGetState);
        const Json&                  jsonState = Json::parse(response.response()).at("state");
        EXPECT_FALSE(jsonState.empty());
        EXPECT_EQ(static_cast<unsigned>(state), jsonState.get<unsigned>());
    }

    template <typename CompT>
    void subscribeToNotification(const message::NotificationId& notificationId)
    {
        m_broker.registerMessageHandler(
            notificationId.getId(),
            [&](const message::Command& command) -> message::CommandResponse {
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_receivedNotifications.push_back(command.name());
                }
                return message::CommandResponse();
            });
        ASSERT_TRUE(
            m_broker.subscribe(notificationId.getPublisherAddress(), notificationId.getId()));
        m_broker.registerPostProcessHandler([&] {
            // Call out of receive context!
            m_condVar.notify_one();
        });
        LOG(debug) << "Subscribed for notification: " << notificationId.getId();
    }

    template <typename CompT>
    void waitForNotification(
        const message::NotificationId&   notificationId,
        const std::chrono::milliseconds& maxTime = std::chrono::milliseconds(60000))
    {
        LOG(debug) << "Expect notification: " << notificationId.getId();
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cv_status               status = std::cv_status::no_timeout;
        while ((status == std::cv_status::no_timeout) &&
               (!hasNotificationReceived(notificationId.getId())))
        {
            status = m_condVar.wait_for(lock, maxTime);
            EXPECT_NE(std::cv_status::timeout, status);
        }
        ASSERT_TRUE(m_broker.unsubscribe(notificationId.getId()));
        m_broker.unregisterMessageHandler(notificationId.getId());
        LOG(debug) << "Expected notification received: " << notificationId.getId();
    }

private:
    bool hasNotificationReceived(const message::MessageId& id)
    {
        auto it = std::find(m_receivedNotifications.cbegin(), m_receivedNotifications.cend(), id);

        if (it == m_receivedNotifications.cend())
        {
            return false;
        }

        m_receivedNotifications.erase(it);
        return true;
    }

    message::Address              m_receiverAddress;
    message::IOContext            m_ioContext;
    message::CommandMessageBroker m_broker;

    unsigned                        m_idCount = 0;
    std::vector<message::MessageId> m_receivedNotifications;
    std::mutex                      m_mutex;
    std::condition_variable         m_condVar;
};

}  // namespace sugo