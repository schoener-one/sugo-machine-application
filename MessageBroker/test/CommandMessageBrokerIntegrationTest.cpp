///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   22.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <istream>
#include <ostream>
#include <thread>

#include <Command.pb.h>

#include "Client.hpp"
#include "CommandMessageBroker.hpp"
#include "IOContext.hpp"
#include "Logger.hpp"

using namespace sugo;

class CommandMessageBrokerIntegrationTest : public ::testing::Test
{
protected:
    static constexpr std::size_t                 NumberOfBrokers = 3;
    inline static const std::vector<std::string> m_brokerIds = {"Broker1", "Broker2", "Broker3"};

    CommandMessageBrokerIntegrationTest()
        : m_ioContext1("IOContext1"),
          m_ioContext2("IOContext2"),
          m_ioContext3("IOContext3"),
          m_broker1(m_brokerIds[0], m_ioContext1),
          m_broker2(m_brokerIds[1], m_ioContext2),
          m_broker3(m_brokerIds[2], m_ioContext3),
          m_brokers({&m_broker1, &m_broker2, &m_broker3})
    {
    }

    ~CommandMessageBrokerIntegrationTest() override
    {
    }

    static void SetUpTestCase()
    {
        Logger::init(Logger::Severity::error);
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

    void sleepFor(std::chrono::seconds seconds)
    {
        std::this_thread::sleep_for(seconds);
    }

    message::Command createCommand(const std::string& name, unsigned index = 0,
                                   const std::string&    parameters = "",
                                   message::Command_Type type       = message::Command_Type_Request)
    {
        message::Command command;
        command.set_id(index);
        command.set_name(name);
        command.set_type(type);
        command.set_parameters(parameters);
        return command;
    }

    IOContext                                          m_ioContext1, m_ioContext2, m_ioContext3;
    CommandMessageBroker                               m_broker1, m_broker2, m_broker3;
    std::array<CommandMessageBroker*, NumberOfBrokers> m_brokers;
};

TEST_F(CommandMessageBrokerIntegrationTest, CommandMessageBroker_RegisterHandler)
{
    message::CommandResponse dummy;
    m_broker1.registerHandler("Handler1", [&](const message::Command&) { return dummy; });
    m_broker1.registerHandler("Handler2", [&](const message::Command&) { return dummy; });
    m_broker1.registerHandler("Handler3", [&](const message::Command&) { return dummy; });
    EXPECT_TRUE(m_broker1.hasRegisteredHandler("Handler2"));
    EXPECT_FALSE(m_broker1.hasRegisteredHandler("Handler4"));
    m_broker1.unregisterHandler("Handler2");
    EXPECT_FALSE(m_broker1.hasRegisteredHandler("Handler2"));
    EXPECT_TRUE(m_broker1.hasRegisteredHandler("Handler3"));
}

TEST_F(CommandMessageBrokerIntegrationTest, CommandMessageBroker_ConnectAndDisconnect)
{
    Client     client(m_ioContext2);
    const auto address = CommandMessageBroker::createInProcessAddress(m_brokerIds.at(0));
    EXPECT_TRUE(client.connect(address));
    EXPECT_TRUE(client.disconnect());
}

TEST_F(CommandMessageBrokerIntegrationTest, CommandMessageBroker_RequestHandler)
{
    Client client(m_ioContext2);
    m_broker1.registerHandler("Wonderful.Day", [&](const message::Command& command) {
        message::CommandResponse dummy;
        dummy.set_id(command.id());
        dummy.set_result(message::CommandResponse_Result_SUCCESS);
        dummy.set_response(command.parameters());
        return dummy;
    });
    EXPECT_TRUE(client.connect(CommandMessageBroker::createInProcessAddress(m_brokerIds.at(0))));
    message::Command command = createCommand("Wonderful.Day", 42, "{ \"param1\": 42 }");
    StreamBuffer     sendBuf, receiveBuf;
    std::ostream     out(&sendBuf);
    EXPECT_TRUE(command.SerializeToOstream(&out));
    EXPECT_TRUE(client.send(sendBuf, receiveBuf));
    std::istream             in(&receiveBuf);
    message::CommandResponse response;
    EXPECT_TRUE(response.ParseFromIstream(&in));
    EXPECT_EQ(response.id(), 42);
    EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
    EXPECT_EQ(response.response(), "{ \"param1\": 42 }");
}

TEST_F(CommandMessageBrokerIntegrationTest, CommandMessageBroker_NotifyAllHandlers)
{
    static const std::string notifName = "hello";
    message::CommandResponse dummy;
    bool                     hasCmb2Received = false, hasCmb3Received = false;
    m_broker2.registerHandler(notifName, [&](const message::Command& command) {
        hasCmb2Received = (command.id() == 42);
        return dummy;
    });
    m_broker3.registerHandler(notifName, [&](const message::Command& command) {
        hasCmb3Received = (command.id() == 42);
        return dummy;
    });
    message::Command notification =
        createCommand(notifName, 42, "", message::Command_Type_Notification);
    EXPECT_TRUE(m_broker1.notify(notification, m_brokerIds));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_TRUE(hasCmb2Received);
    EXPECT_TRUE(hasCmb3Received);
}

TEST_F(CommandMessageBrokerIntegrationTest, CommandMessageBroker_PostRequestProcessing)
{
    static const std::string requestId1 = "request-1";
    static const std::string requestId2 = "request-2";
    static const std::string responseId2 = "response-2";
    static const std::string responseId1 = "response-1";
    
    bool hasReceived = false;
    m_broker2.registerPostProcessHandler([&] {
        message::CommandResponse response;
        EXPECT_TRUE(m_broker2.send(createCommand(requestId1), m_brokerIds.at(0), response));
        EXPECT_EQ(response.response(), responseId1);
    });
    m_broker2.registerHandler(requestId2, [&](const message::Command&) {
        message::CommandResponse response;
        response.set_response(responseId2);
        return response;
    });
    m_broker1.registerHandler(requestId1, [&](const message::Command&) {
        hasReceived = true;
        message::CommandResponse response;
        response.set_response(responseId1);
        return response;
    });
    message::CommandResponse response;
    EXPECT_TRUE(m_broker1.send(createCommand(requestId2), m_brokerIds.at(1), response));
    std::string s = response.response();
    EXPECT_EQ(response.response(), responseId2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_TRUE(hasReceived);
}

TEST_F(CommandMessageBrokerIntegrationTest, CommandMessageBroker_ForbitSendingDuringRequestProcessing)
{
    static const std::string requestId1 = "request-1";
    static const std::string requestId2 = "request-2";
    static const std::string responseId2 = "response-2";
    static const std::string responseId1 = "response-1";
    
    bool hasReceived = false;
    m_broker2.registerHandler(requestId2, [&](const message::Command&) {
        message::CommandResponse response;
        EXPECT_FALSE(m_broker2.send(createCommand(requestId1), m_brokerIds.at(0), response));
        response.set_response(responseId2);
        return response;
    });
    m_broker1.registerHandler(requestId1, [&](const message::Command&) {
        hasReceived = true;
        message::CommandResponse response;
        response.set_response(responseId1);
        return response;
    });
    message::CommandResponse response;
    EXPECT_TRUE(m_broker1.send(createCommand(requestId2), m_brokerIds.at(1), response));
    std::string s = response.response();
    EXPECT_EQ(response.response(), responseId2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(hasReceived);
}