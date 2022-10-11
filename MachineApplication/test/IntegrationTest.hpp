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

#include "CommandMessageBroker.hpp"
#include "Logger.hpp"
#include "ServiceComponent.hpp"

#include <Command.pb.h>

/// Expect a defined state of that component
#define EXPECT_STATE(_COMP, _STATE) expectState<_COMP>(_COMP::State::_STATE)
#define EXPECT_NOTIFICATION(_COMP, _NOTIF) waitForNotification<_COMP>(_COMP::_NOTIF)

namespace sugo
{
class IntegrationTest : public ::testing::Test
{
protected:
    IntegrationTest(const ICommandMessageBroker::ReceiverId& receiverId)
        : m_receiverId(receiverId), m_ioContext(receiverId), m_broker(receiverId, m_ioContext)
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

    message::CommandResponse send(const ServiceComponent::CommandId& commandId)
    {
        message::Command command;
        command.set_id(m_idCount);
        command.set_name(commandId.getId());
        message::CommandResponse response;
        EXPECT_TRUE(m_broker.send(command, commandId.getReceiverId(), response));
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
        auto        response  = send(CompT::CommandGetState);
        const Json& jsonState = Json::parse(response.response()).at("state");
        EXPECT_FALSE(jsonState.empty());
        EXPECT_EQ(static_cast<unsigned>(state), jsonState.get<unsigned>());
    }

    template <typename CompT>
    void waitForNotification(
        const ServiceComponent::NotificationId& notificationId,
        const std::chrono::milliseconds&        maxTime = std::chrono::milliseconds(30000))
    {
        m_broker.registerHandler(notificationId.getId(),
                                 [&](const message::Command& command) -> message::CommandResponse {
                                     std::unique_lock<std::mutex> lock(m_mutex);
                                     m_lastCommandId = command.name();
                                     lock.unlock();
                                     m_condVar.notify_one();
                                     lock.lock();
                                     return message::createResponse(command);
                                 });

        LOG(debug) << "Expect notification: " << notificationId.getId();
        std::unique_lock<std::mutex> lock(m_mutex);
        m_lastCommandId.clear();
        std::cv_status status = std::cv_status::timeout;
        do
        {
            status = m_condVar.wait_for(lock, maxTime);
        } while ((status == std::cv_status::no_timeout) &&
                 (m_lastCommandId != notificationId.getId()));
        EXPECT_NE(std::cv_status::timeout, status);
        EXPECT_EQ(m_lastCommandId, notificationId.getId());
        m_broker.unregisterHandler(notificationId.getId());
        LOG(debug) << "Expected notification received: " << notificationId.getId();
    }

    ICommandMessageBroker::ReceiverId m_receiverId;
    IOContext                         m_ioContext;
    CommandMessageBroker              m_broker;

    unsigned                m_idCount = 0;
    std::string             m_lastCommandId;
    std::mutex              m_mutex;
    std::condition_variable m_condVar;
};

}  // namespace sugo