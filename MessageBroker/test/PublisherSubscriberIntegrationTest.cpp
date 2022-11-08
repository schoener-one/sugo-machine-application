///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis
 * @date:   18.10.2022
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

#include "IntegrationTest.hpp"
#include "Logger.hpp"
#include "Publisher.hpp"
#include "StreamBuffer.hpp"
#include "Subscriber.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::WithArgs;

namespace sugo::message
{
class MessageHandlerMock : public IMessageHandler
{
public:
    MOCK_METHOD(bool, processReceived, (StreamBuffer&, StreamBuffer&));
    MOCK_METHOD(void, processPost, ());

    std::vector<std::string> m_receivedMessages;
};
}  // namespace sugo::message

using namespace sugo::message;
using namespace sugo;

class PublisherSubscriberIntegrationTest : public IntegrationTest
{
protected:
    static constexpr size_t NumberOfSubscribers = 4;
    static constexpr size_t NumberOfPublishers  = Subscriber::MaxSubscriptionSockets + 1;

    inline static const std::string pubBaseAddr{"inproc://publisher"};

    PublisherSubscriberIntegrationTest()
        : m_pubContext({IOContext{"publisher0"}, IOContext{"publisher1"}, IOContext{"publisher2"},
                        IOContext{"publisher3"}, IOContext{"publisher4"}, IOContext{"publisher5"},
                        IOContext{"publisher6"}, IOContext{"publisher7"}, IOContext{"publisher8"}}),
          m_subContext({IOContext{"subscriber0"}, IOContext{"subscriber1"},
                        IOContext{"subscriber2"}, IOContext{"subscriber3"}}),
          m_publisher({Publisher{pubBaseAddr + "0", m_pubContext[0]},
                       Publisher{pubBaseAddr + "1", m_pubContext[1]},
                       Publisher{pubBaseAddr + "2", m_pubContext[2]},
                       Publisher{pubBaseAddr + "3", m_pubContext[3]},
                       Publisher{pubBaseAddr + "4", m_pubContext[4]},
                       Publisher{pubBaseAddr + "5", m_pubContext[5]},
                       Publisher{pubBaseAddr + "6", m_pubContext[6]},
                       Publisher{pubBaseAddr + "7", m_pubContext[7]},
                       Publisher{pubBaseAddr + "8", m_pubContext[8]}}),
          m_subscriber({Subscriber{m_messageHandlerMock, m_subContext[0]},
                        Subscriber{m_messageHandlerMock, m_subContext[1]},
                        Subscriber{m_messageHandlerMock, m_subContext[2]},
                        Subscriber{m_messageHandlerMock, m_subContext[3]}})
    {
    }

    ~PublisherSubscriberIntegrationTest() override
    {
    }

    static void SetUpTestCase()
    {
        Logger::init(Logger::Severity::debug);
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

    bool publishMessage(Publisher& publisher, const MessageId& topic, const std::string& message)
    {
        StreamBuffer outBuf;
        std::ostream os(&outBuf);
        os << message;
        const bool success = publisher.publish(topic, outBuf);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));  // Just delay to make this message processed first!
        return success;
    }

    void receiveMessage(StreamBuffer& inBuf)
    {
        std::istream in(&inBuf);
        std::string  message;
        std::getline(in, message);
        notifyReceivedMessage(message);
    }

    // reused tests
    void testSubscribeToMultiplePublisherAndReceiveMultipleMessages();

    MessageHandlerMock                          m_messageHandlerMock;
    std::array<IOContext, NumberOfPublishers>   m_pubContext;
    std::array<IOContext, NumberOfSubscribers>  m_subContext;
    std::array<Publisher, NumberOfPublishers>   m_publisher;
    std::array<Subscriber, NumberOfSubscribers> m_subscriber;
    StreamBuffer                                m_outBuf;
};

TEST_F(PublisherSubscriberIntegrationTest, PublisherStartStop)
{
    EXPECT_TRUE(m_publisher[0].isRunning());
    m_publisher[0].stop();
    EXPECT_FALSE(m_publisher[0].isRunning());
}

TEST_F(PublisherSubscriberIntegrationTest, PublishWithoutSubscriber)
{
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic0", "test-message"));
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeAndUnsubscribeToPublisher)
{
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[0].getAddress(), "topic0"));
    EXPECT_CALL(m_messageHandlerMock, processReceived(_, _))
        .WillOnce(WithArgs<0>(Invoke([&](StreamBuffer& inBuf) {
            receiveMessage(inBuf);
            return true;
        })));
    EXPECT_CALL(m_messageHandlerMock, processPost()).Times(1);
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic0", "test-message"));
    waitForReceivedMessages(1);
    EXPECT_EQ(m_messageReceiveQueue.front(), "test-message");
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeToMultiplePublisherAndReceiveOneNotification)
{
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[0].getAddress(), "topic0"));
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[1].getAddress(), "topic1"));
    EXPECT_CALL(m_messageHandlerMock, processReceived(_, _))
        .WillOnce(WithArgs<0>(Invoke([&](StreamBuffer& inBuf) {
            receiveMessage(inBuf);
            return true;
        })));
    EXPECT_CALL(m_messageHandlerMock, processPost()).Times(1);
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic1", "test-message-0"));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic0", "test-message-1"));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic1",
                               "test-message-2"));  // should be the received message!
    waitForReceivedMessages(1);
    EXPECT_EQ(m_messageReceiveQueue.front(), "test-message-2");
}

void PublisherSubscriberIntegrationTest::
    testSubscribeToMultiplePublisherAndReceiveMultipleMessages()
{
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[0].getAddress(), "topic0"));
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[1].getAddress(), "topic1"));
    EXPECT_CALL(m_messageHandlerMock, processReceived(_, _))
        .Times(2)
        .WillRepeatedly(WithArgs<0>(Invoke([&](StreamBuffer& inBuf) {
            receiveMessage(inBuf);
            return true;
        })));
    EXPECT_CALL(m_messageHandlerMock, processPost()).Times(2);
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic0",
                               "test-message-0"));  // should be the received message!
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic0", "test-message-1"));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic1",
                               "test-message-2"));  // should be the received message!
    waitForReceivedMessages(2);
    EXPECT_EQ(m_messageReceiveQueue.front(), "test-message-0");
    m_messageReceiveQueue.pop();
    EXPECT_EQ(m_messageReceiveQueue.front(), "test-message-2");
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
    EXPECT_TRUE(m_subscriber[0].unsubscribe("topic3"));
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

    EXPECT_TRUE(m_subscriber[0].unsubscribe("topic0"));
    EXPECT_TRUE(m_subscriber[0].unsubscribe("topic1"));

    // should not be received anymore!
    EXPECT_TRUE(publishMessage(m_publisher[0], "topic0", "test-message-0"));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic0", "test-message-1"));
    EXPECT_TRUE(publishMessage(m_publisher[1], "topic1", "test-message-2"));
}

TEST_F(PublisherSubscriberIntegrationTest, SubscribeAndPublishFromTwoInstances)
{
    EXPECT_TRUE(m_subscriber[0].subscribe(m_publisher[1].getAddress(), "pub1_topic"));
    EXPECT_TRUE(m_subscriber[1].subscribe(m_publisher[0].getAddress(), "pub0_topic"));
    EXPECT_CALL(m_messageHandlerMock, processReceived(_, _))
        .Times(2)
        .WillRepeatedly(WithArgs<0>(Invoke([&](StreamBuffer& inBuf) {
            receiveMessage(inBuf);
            return true;
        })));
    EXPECT_CALL(m_messageHandlerMock, processPost()).Times(2);
    EXPECT_TRUE(publishMessage(m_publisher[1], "pub1_topic", "test-message-1"));
    EXPECT_TRUE(publishMessage(m_publisher[0], "pub0_topic", "test-message-2"));
    waitForReceivedMessages(2);
    EXPECT_EQ(m_messageReceiveQueue.front(), "test-message-1");
    m_messageReceiveQueue.pop();
    EXPECT_EQ(m_messageReceiveQueue.front(), "test-message-2");
}