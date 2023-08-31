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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <istream>
#include <ostream>
#include <thread>

#include "Common/IOContext.hpp"
#include "Common/Logger.hpp"
#include "IntegrationTest.hpp"
#include "MessageBroker/Client.hpp"
#include "MessageBroker/MessageBroker.hpp"

using namespace sugo::message_broker;
using namespace sugo;

class MessageBrokerIntegrationTest : public IntegrationTest
{
protected:
    static constexpr std::size_t                 NumberOfBrokers = 3;
    inline static const std::vector<std::string> m_brokerIds = {"Broker1", "Broker2", "Broker3"};

    MessageBrokerIntegrationTest()
        : m_ioContext1("Context1"),
          m_ioContext2("Context2"),
          m_ioContext3("Context3"),
          m_broker1(m_brokerIds[0], m_ioContext1),
          m_broker2(m_brokerIds[1], m_ioContext2),
          m_broker3(m_brokerIds[2], m_ioContext3),
          m_brokers({&m_broker1, &m_broker2, &m_broker3})
    {
    }

    void SetUp() override
    {
        for (auto& broker : m_brokers)
        {
            ASSERT_TRUE(broker->start());
        }
    }

    void TearDown() override
    {
        for (auto& broker : m_brokers)
        {
            if (broker->isRunning())
            {
                broker->stop();
            }
        }
    }

    static Message createMessage(Message::Identifier id, unsigned index = 0,
                                 const std::string& payload = "")
    {
        Message message{};
        message.setSequence(index);
        message.setId(id);
        message.setPayload(payload);
        return message;
    }

    common::IOContext                           m_ioContext1, m_ioContext2, m_ioContext3;
    MessageBroker                               m_broker1, m_broker2, m_broker3;
    std::array<MessageBroker*, NumberOfBrokers> m_brokers;
};

TEST_F(MessageBrokerIntegrationTest, RegisterHandler)
{
    m_broker1.registerRequestMessageHandler(1, [&](const Message&) { return ResponseMessage{}; });
    m_broker1.registerRequestMessageHandler(2, [&](const Message&) { return ResponseMessage{}; });
    m_broker1.registerRequestMessageHandler(3, [&](const Message&) { return ResponseMessage{}; });
    EXPECT_TRUE(m_broker1.hasRegisteredMessageHandler(2));
    EXPECT_FALSE(m_broker1.hasRegisteredMessageHandler(4));
    m_broker1.unregisterMessageHandler(2);
    EXPECT_FALSE(m_broker1.hasRegisteredMessageHandler(2));
    EXPECT_TRUE(m_broker1.hasRegisteredMessageHandler(3));
}

TEST_F(MessageBrokerIntegrationTest, ConnectAndDisconnect)
{
    Client     client(m_ioContext2);
    const auto address = MessageBroker::createFullQualifiedAddress(
        m_brokerIds.at(0), MessageBroker::Service::Responder);
    EXPECT_TRUE(client.connect(address));
    EXPECT_TRUE(client.disconnect());
}

TEST_F(MessageBrokerIntegrationTest, RequestHandler)
{
    constexpr Message::Identifier messageId = 23;

    Client client(m_ioContext2);
    m_broker1.registerRequestMessageHandler(messageId, [&](const Message& message) {
        ResponseMessage response;
        response.referTo(message);
        response.setResult(ResponseMessage::Result::Success);
        response.setError(message.getPayload());
        return response;
    });
    EXPECT_TRUE(client.connect(MessageBroker::createFullQualifiedAddress(
        m_brokerIds.at(0), MessageBroker::Service::Responder)));
    Message      message = createMessage(messageId, 42, "the error reason");
    StreamBuffer sendBuf, receiveBuf;
    std::ostream out(&sendBuf);
    EXPECT_TRUE(message.serialize(out));
    EXPECT_TRUE(client.send(sendBuf, receiveBuf));
    std::istream    in(&receiveBuf);
    ResponseMessage response;
    EXPECT_TRUE(response.deserialize(in));
    EXPECT_EQ(response.getId(), messageId);
    EXPECT_EQ(response.getSequence(), 42);
    EXPECT_EQ(response.getResult(), ResponseMessage::Result::Success);
    EXPECT_EQ(response.getError(), "{\"reason\":\"the error reason\"}");
}

TEST_F(MessageBrokerIntegrationTest, NotifyAllHandlers)
{
    static const Message::Identifier messageId = 0x42;

    m_broker2.registerNotificationMessageHandler(messageId,
                                                 [&](const Message& message) -> ResponseMessage {
                                                     notifyReceivedMessage(message.getId());
                                                     return ResponseMessage();
                                                 });
    Topic theTopic{"the_topic"};
    EXPECT_TRUE(m_broker2.subscribe(m_brokerIds[0], theTopic));
    m_broker3.registerNotificationMessageHandler(messageId, [&](const Message& message) {
        notifyReceivedMessage(message.getId());
        return ResponseMessage();
    });
    EXPECT_TRUE(m_broker3.subscribe(m_brokerIds[0], theTopic));
    Message notification = createMessage(messageId, 42, "");
    EXPECT_TRUE(m_broker1.notify(notification, theTopic));
    waitForReceivedMessages(2);
    EXPECT_EQ(m_messageReceiveQueue.front(), messageId);
    m_messageReceiveQueue.pop();
    EXPECT_EQ(m_messageReceiveQueue.front(), messageId);
}
