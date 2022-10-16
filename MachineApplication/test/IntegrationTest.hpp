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
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "CommandMessageBroker.hpp"
#include "Logger.hpp"
#include "ServiceComponent.hpp"

#include <Command.pb.h>

/// Expect a defined state of that component
#define EXPECT_STATE(_COMP, _STATE) expectState<_COMP>(_COMP::State::_STATE)
#define EXPECT_NOTIFICATION_BEGIN(_COMP, _NOTIF) subscribeToNotification<_COMP>(_COMP::_NOTIF)
#define EXPECT_NOTIFICATION_END(_COMP, _NOTIF) waitForNotification<_COMP>(_COMP::_NOTIF)

namespace sugo
{
class IntegrationTest : public ::testing::Test
{
protected:
    IntegrationTest(const message::Address& address)
        : m_receiverAddress(address), m_ioContext(address), m_broker(address, m_ioContext)
    {
    }

    void SetUp() override
    {
        m_lastCommandId.clear();
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
        m_lastCommandId.clear();
        m_broker.registerMessageHandler(
            notificationId.getId(),
            [&](const message::Command& command) -> message::CommandResponse {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_lastCommandId = command.name();
                lock.unlock();
                m_condVar.notify_one();
                return message::createCommandResponse(command);
            });
        ASSERT_TRUE(
            m_broker.subscribe(notificationId.getPublisherAddress(), notificationId.getId()));
        LOG(debug) << "Subscribed for notification: " << notificationId.getId();
    }

    template <typename CompT>
    void waitForNotification(
        const message::NotificationId&   notificationId,
        const std::chrono::milliseconds& maxTime = std::chrono::milliseconds(30000))
    {
        LOG(debug) << "Expect notification: " << notificationId.getId();
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cv_status status = std::cv_status::timeout;
        do
        {
            status = m_condVar.wait_for(lock, maxTime);
        } while ((status == std::cv_status::no_timeout) &&
                 (m_lastCommandId != notificationId.getId()));
        EXPECT_NE(std::cv_status::timeout, status);
        EXPECT_EQ(m_lastCommandId, notificationId.getId());
        ASSERT_TRUE(m_broker.unsubscribe(notificationId.getId()));
        m_broker.unregisterMessageHandler(notificationId.getId());
        LOG(debug) << "Expected notification received: " << notificationId.getId();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    message::Address              m_receiverAddress;
    message::IOContext            m_ioContext;
    message::CommandMessageBroker m_broker;

    unsigned                m_idCount = 0;
    std::string             m_lastCommandId;
    std::mutex              m_mutex;
    std::condition_variable m_condVar;
};

}  // namespace sugo