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
#include "Logger.hpp"
#include "Server.hpp"

namespace bc = boost::container;

namespace sugo
{
class EchoMessageHandler : public Server::IMessageHandler
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

    void processPost()
    {
    }

    bc::vector<std::string> m_receivedMessages;
};
}  // namespace sugo

using namespace sugo;

class ClientServerIntegrationTest : public ::testing::Test
{
protected:
    const std::string addressServer = "inproc://server";
    
    ClientServerIntegrationTest()
        : m_serverContext("server"),
          m_clientContext("client"),
          m_server(addressServer, m_messageHandler, m_serverContext),
          m_client(m_clientContext)
    {
    }

    ~ClientServerIntegrationTest() override
    {
    }

    static void SetUpTestCase()
    {
        Logger::init(Logger::Severity::trace);
    }

    void SetUp() override
    {
        ASSERT_TRUE(m_server.start());
        ASSERT_TRUE(m_serverContext.start());
        ASSERT_TRUE(m_clientContext.start());
    }

    void TearDown() override
    {
        if (m_clientContext.isRunning())
        {
            m_clientContext.stop();
        }
        if (m_serverContext.isRunning())
        {
            m_serverContext.stop();
        }
        if (m_server.isRunning())
        {
            m_server.stop();
        }
    }

    static std::string sendMessage(const std::string& message, Client& client);

    EchoMessageHandler m_messageHandler;
    IOContext      m_serverContext, m_clientContext;
    Server         m_server;
    Client         m_client;
    StreamBuffer   m_outBuf;
};

std::string ClientServerIntegrationTest::sendMessage(const std::string& message, Client& client)
{
    StreamBuffer outBuf;
    std::ostream os(&outBuf);
    os << message;
    StreamBuffer inBuf;
    std::istream is(&inBuf);
    std::string  response;
    EXPECT_TRUE(client.send(outBuf, inBuf));
    std::getline(is, response);
    return response;
}

TEST_F(ClientServerIntegrationTest, Server_StartStop)
{
    EXPECT_TRUE(m_server.isRunning());
    m_server.stop();
    EXPECT_FALSE(m_server.isRunning());
}

TEST_F(ClientServerIntegrationTest, Server_ReplyRequest)
{
    EXPECT_TRUE(m_client.connect(addressServer));
    static const std::string message  = "How are you?";
    const std::string        response = sendMessage(message, m_client);
    EXPECT_TRUE(m_client.disconnect(addressServer));
    EXPECT_EQ(response, message);
}

TEST_F(ClientServerIntegrationTest, Server_RequestQueueing)
{
    // Prepare clients
    std::function<void(int)> sender([=](int index) {
        Logger::reinit();
        IOContext clientIoContext("client");
        Client    client(clientIoContext);
        clientIoContext.start();
        EXPECT_TRUE(client.connect(addressServer));
        const std::string message = std::to_string(index);
        (void)sendMessage(message, client);
        clientIoContext.stop();
    });

    // Run test
    std::thread clientThread1(sender, 1);
    std::thread clientThread2(sender, 2);
    std::thread clientThread3(sender, 3);
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
