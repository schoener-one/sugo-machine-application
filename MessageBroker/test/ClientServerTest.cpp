///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   29.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <istream>
#include <ostream>
#include <thread>

#include <boost/container/vector.hpp>

#include "Client.hpp"
#include "IOContextHelper.hpp"
#include "Logger.hpp"
#include "Server.hpp"

namespace bc = boost::container;

namespace sugo
{
class TestEchoServer : public Server::IMessageHandler
{
public:
    bool processReceived(StreamBuffer& inBuf, StreamBuffer& outBuf) override
    {
        std::istream in(&inBuf);
        std::string  receivedMessage;
        std::getline(in, receivedMessage);
        m_receivedMessages.push_back(receivedMessage);
        std::ostream out(&outBuf);
        out << receivedMessage;
        return true;
    }

    bc::vector<std::string> m_receivedMessages;
};

const std::string address = "inproc://server";
}  // namespace sugo

using namespace sugo;

class ClientServerTest : public ::testing::Test, public IOContextHelper
{
protected:
    ClientServerTest()
        : IOContextHelper(), m_server(address, m_messageHandler, m_ioContext), m_client(m_ioContext)
    {
    }

    ~ClientServerTest() override {}

    static void SetUpTestCase() { Logger::init(Logger::Severity::error); }

    void SetUp() override {}

    void TearDown() override { stopIOContext(); }

    static std::string sendMessage(const std::string& message, Client& client,
                                   bool isNotification = false)
    {
        StreamBuffer outBuf;
        std::ostream os(&outBuf);
        os << message;
        StreamBuffer inBuf;
        std::istream is(&inBuf);
        std::string  response;
        if (isNotification)
        {
            EXPECT_TRUE(client.notify(outBuf));
        }
        else
        {
            EXPECT_TRUE(client.send(outBuf, inBuf));
            std::getline(is, response);
        }
        return response;
    }

    void testReceiveQueue(bool testNotificationQueue);

    TestEchoServer m_messageHandler;
    Server         m_server;
    Client         m_client;
    StreamBuffer   m_outBuf;
};

TEST_F(ClientServerTest, Server_StartStop)
{
    EXPECT_TRUE(m_server.start());
    EXPECT_TRUE(m_server.isRunning());
    m_server.stop();
    EXPECT_FALSE(m_server.isRunning());
}

TEST_F(ClientServerTest, Server_ReplyRequest)
{
    EXPECT_TRUE(m_server.start());
    startIOContext();
    EXPECT_TRUE(m_client.connect(address));
    static const std::string message  = "How are you?";
    const std::string        response = sendMessage(message, m_client);
    EXPECT_TRUE(m_client.disconnect(address));
    EXPECT_EQ(response, message);
}

void ClientServerTest::testReceiveQueue(bool testNotificationQueue)
{
    // Prepare server
    EXPECT_TRUE(m_server.start());
    m_ioContextMutex.lock();
    startIOContext();

    // Prepare clients
    std::function<void(int)> sender([=](int index) {
        IOContext clientIoContext;
        Client    client(clientIoContext);
        clientIoContext.start();
        EXPECT_TRUE(client.connect(address));
        const std::string message = std::to_string(index);
        (void)sendMessage(message, client, testNotificationQueue);
    });

    // Run test
    std::thread clientThread1(sender, 1);
    std::thread clientThread2(sender, 2);
    std::thread clientThread3(sender, 3);
    m_ioContextMutex.unlock();  // Starting server processing
    clientThread1.join();
    clientThread2.join();
    clientThread3.join();
    std::sort(m_messageHandler.m_receivedMessages.begin(),
              m_messageHandler.m_receivedMessages.end());
    const unsigned numReceived = m_messageHandler.m_receivedMessages.size();
    EXPECT_EQ(numReceived, 3);
    EXPECT_EQ(m_messageHandler.m_receivedMessages.at(0), "1");
    EXPECT_EQ(m_messageHandler.m_receivedMessages.at(1), "2");
    EXPECT_EQ(m_messageHandler.m_receivedMessages.at(2), "3");
}

TEST_F(ClientServerTest, Server_RequestQueueing) { testReceiveQueue(false); }

TEST_F(ClientServerTest, Server_NotificationQueueing) { testReceiveQueue(true); }
