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
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

#include "Common/Logger.hpp"
#include "IntegrationTest.hpp"
#include "MessageBroker/Publisher.hpp"
#include "MessageBroker/StreamBuffer.hpp"
#include "MessageBroker/Subscriber.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::WithArgs;

using namespace sugo::message_broker;
using namespace sugo;

/// @brief The test message
struct TestMessage
{
    Message::Identifier id = 0;
    std::string         payload{};
};

/// @brief Equal operator.
/// @param message1 Message 1 to compare.
/// @param message2 Message 2 to compare.
/// @return True if equal.
bool operator==(const TestMessage& message1, const TestMessage& message2)
{
    return ((message1.id == message2.id) and (message1.payload == message2.payload));
}

/// @brief Test class
class PublisherSubscriberIntegrationTest : public IntegrationTest
{
protected:
    static constexpr size_t NumberOfSubscribers = 4;
    static constexpr size_t NumberOfPublishers  = Subscriber::MaxSubscriptionSockets + 1;

    inline static const std::string pubBaseAddr{"inproc://publisher"};

    PublisherSubscriberIntegrationTest()
        : m_pubContext({common::IOContext{"publisher0"}, common::IOContext{"publisher1"},
                        common::IOContext{"publisher2"}, common::IOContext{"publisher3"},
                        common::IOContext{"publisher4"}, common::IOContext{"publisher5"},
                        common::IOContext{"publisher6"}, common::IOContext{"publisher7"},
                        common::IOContext{"publisher8"}}),
          m_subContext({common::IOContext{"subscriber0"}, common::IOContext{"subscriber1"},
                        common::IOContext{"subscriber2"}, common::IOContext{"subscriber3"}}),
          m_publisher({Publisher{pubBaseAddr + "0", m_pubContext[0]},
                       Publisher{pubBaseAddr + "1", m_pubContext[1]},
                       Publisher{pubBaseAddr + "2", m_pubContext[2]},
                       Publisher{pubBaseAddr + "3", m_pubContext[3]},
                       Publisher{pubBaseAddr + "4", m_pubContext[4]},
                       Publisher{pubBaseAddr + "5", m_pubContext[5]},
                       Publisher{pubBaseAddr + "6", m_pubContext[6]},
                       Publisher{pubBaseAddr + "7", m_pubContext[7]},
                       Publisher{pubBaseAddr + "8", m_pubContext[8]}}),
          m_subscriber({Subscriber{m_messageHandlerMock.AsStdFunction(), m_subContext[0]},
                        Subscriber{m_messageHandlerMock.AsStdFunction(), m_subContext[1]},
                        Subscriber{m_messageHandlerMock.AsStdFunction(), m_subContext[2]},
                        Subscriber{m_messageHandlerMock.AsStdFunction(), m_subContext[3]}})
    {
    }

    ~PublisherSubscriberIntegrationTest() override
    {
    }

    static void SetUpTestCase()
    {
        common::Logger::init(common::Logger::Severity::debug);
    }

    void SetUp() override
    {
        for (size_t i = 0; i < NumberOfPublishers; i++)
        {
            ASSERT_TRUE(m_publisher[i].start());
            ASSERT_TRUE(m_pubContext[i].start());
        }
        for (size_t i = 0; i < NumberOfSubscribers; i++)
        {
            ASSERT_TRUE(m_subContext[i].start());
        }
    }

    void TearDown() override
    {
        for (size_t i = 0; i < NumberOfSubscribers; i++)
        {
            if (m_subContext[i].isRunning())
            {
                m_subContext[i].stop();
            }
        }
        for (size_t i = 0; i < NumberOfPublishers; i++)
        {
            if (m_pubContext[i].isRunning())
            {
                m_pubContext[i].stop();
            }
            if (m_publisher[i].isRunning())
            {
                m_publisher[i].stop();
            }
        }
    }

    bool publishMessage(Publisher& publisher, const Topic& topic, const TestMessage& message)
    {
        StreamBuffer outBuf;
        std::ostream os(&outBuf);
        os.write(reinterpret_cast<const char*>(&message.id), sizeof(message.id));
        os << message.payload;
        os.flush();
        const bool success = publisher.publish(topic, outBuf);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));  // Just delay to make this message processed first!
        return success;
    }

    TestMessage receiveMessage(StreamBuffer& inBuf)
    {
        std::istream is(&inBuf);
        TestMessage  message{};
        is.read(reinterpret_cast<char*>(&message.id), sizeof(message.id));
        is >> message.payload;
        notifyReceivedMessage(message.id);
        return message;
    }

    // reused tests
    void                              testSubscribeToMultiplePublisherAndReceiveMultipleMessages();
    MockFunction<bool(StreamBuffer&)> m_messageHandlerMock;
    std::array<common::IOContext, NumberOfPublishers>  m_pubContext;
    std::array<common::IOContext, NumberOfSubscribers> m_subContext;
    std::array<Publisher, NumberOfPublishers>          m_publisher;
    std::array<Subscriber, NumberOfSubscribers>        m_subscriber;
    StreamBuffer                                       m_outBuf;
};

TEST_F(PublisherSubscriberIntegrationTest, PublisherStartStop)
{
    EXPECT_TRUE(m_publisher[0].isRunning());
    m_publisher[0].stop();
    EXPECT_FALSE(m_publisher[0].isRunning());
}

TEST_F(PublisherSubscriberIntegrationTest, PublishWithoutSubscriber)
{
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic1", {1, "test-message"}));
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeAndUnsubscribeToPublisher)
{
    static const TestMessage testMessage1{1, "test-message-1"};

    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[0].getAddress(), "topic1"));
    EXPECT_CALL(m_messageHandlerMock, Call(_))
        .WillOnce(WithArgs<0>(Invoke([&](StreamBuffer& inBuf) {
            const auto message = receiveMessage(inBuf);
            EXPECT_EQ(message, testMessage1);
            return true;
        })));
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic1", testMessage1));
    waitForReceivedMessages(1);
    EXPECT_EQ(m_messageReceiveQueue.front(), testMessage1.id);
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeToMultiplePublisherAndReceiveOneNotification)
{
    static const TestMessage testMessage1{1, "test-message-1"};
    static const TestMessage testMessage2{2, "test-message-2"};
    static const TestMessage testMessage3{3, "test-message-3"};

    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[0].getAddress(), "topic1"));
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[1].getAddress(), "topic2"));
    EXPECT_CALL(m_messageHandlerMock, Call(_))
        .WillOnce(WithArgs<0>(Invoke([&](StreamBuffer& inBuf) {
            const auto message = receiveMessage(inBuf);
            return true;
        })));
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic2", testMessage2));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic1", testMessage1));
    EXPECT_TRUE(
        publishMessage(m_publisher[1], "topic2", testMessage3));  // should be the received message!

    waitForReceivedMessages(1);
    EXPECT_EQ(m_messageReceiveQueue.front(), testMessage3.id);
}

void PublisherSubscriberIntegrationTest::
    testSubscribeToMultiplePublisherAndReceiveMultipleMessages()
{
    static const TestMessage testMessage1{1, "test-message-1"};
    static const TestMessage testMessage2{2, "test-message-2"};
    static const TestMessage testMessage3{3, "test-message-3"};

    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[0].getAddress(), "topic1"));
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[1].getAddress(), "topic2"));
    EXPECT_CALL(m_messageHandlerMock, Call(_))
        .Times(2)
        .WillRepeatedly(WithArgs<0>(Invoke([&](StreamBuffer& inBuf) {
            receiveMessage(inBuf);
            return true;
        })));
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic1",
                               testMessage1));  // should be the received message!
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic1", testMessage2));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic2",
                               testMessage3));  // should be the received message!
    waitForReceivedMessages(2);
    EXPECT_EQ(m_messageReceiveQueue.front(), testMessage1.id);
    m_messageReceiveQueue.pop();
    EXPECT_EQ(m_messageReceiveQueue.front(), testMessage3.id);
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeToMultiplePublisherAndReceiveMultipleMessages)
{
    testSubscribeToMultiplePublisherAndReceiveMultipleMessages();
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeToTooMuchPublisherAddresses)
{
    for (size_t i = 0; i < Subscriber::MaxSubscriptionSockets; i++)
    {
        EXPECT_TRUE(
            m_subscriber[0].subscribe(m_publisher[i].getAddress(), "topic" + std::to_string(i)));
    }
    EXPECT_FALSE(
        m_subscriber[0].subscribe(m_publisher[Subscriber::MaxSubscriptionSockets].getAddress(),
                                  "topic" + std::to_string(Subscriber::MaxSubscriptionSockets)));
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeAfterUnsubscribeWithOneRemainingSocket)
{
    for (size_t i = 0; i < Subscriber::MaxSubscriptionSockets; i++)
    {
        EXPECT_TRUE(
            m_subscriber[0].subscribe(m_publisher[i].getAddress(), "topic" + std::to_string(i)));
    }
    EXPECT_TRUE(m_subscriber[0].unsubscribe(m_publisher[3].getAddress(), "topic3"));
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[2].getAddress(), "topic"));
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeToSameTopicTwiceNotAllowed)
{
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[0].getAddress(), "topic"));
    EXPECT_FALSE(m_subscriber[0].subscribe(m_publisher[0].getAddress(), "topic"));
}

TEST_F(PublisherSubscriberIntegrationTest,
       UnsubscribeFromOnePublisherAfterReceivingMultipleNotifications)
{
    testSubscribeToMultiplePublisherAndReceiveMultipleMessages();

    EXPECT_TRUE(m_subscriber[0].unsubscribe(m_publisher[0].getAddress(), "topic1"));
    EXPECT_TRUE(m_subscriber[0].unsubscribe(m_publisher[1].getAddress(), "topic2"));

    // should not be received anymore!
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic1", {1, "test-message-1"}));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic1", {2, "test-message-2"}));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic2", {3, "test-message-3"}));
}
