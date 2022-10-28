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
#include "IntegrationTest.hpp"
#include "Logger.hpp"

using namespace sugo::message;
using namespace sugo;

class CommandMessageBrokerIntegrationTest : public IntegrationTest
{
protected:
    static constexpr std::size_t                 NumberOfBrokers = 3;
    inline static const std::vector<std::string> m_brokerIds = {"Broker1", "Broker2", "Broker3"};

    CommandMessageBrokerIntegrationTest()
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

TEST_F(CommandMessageBrokerIntegrationTest, RegisterHandler)
{
    message::CommandResponse dummy;
    m_broker1.registerMessageHandler("Handler1", [&](const message::Command&) { return dummy; });
    m_broker1.registerMessageHandler("Handler2", [&](const message::Command&) { return dummy; });
    m_broker1.registerMessageHandler("Handler3", [&](const message::Command&) { return dummy; });
    EXPECT_TRUE(m_broker1.hasRegisteredMessageHandler("Handler2"));
    EXPECT_FALSE(m_broker1.hasRegisteredMessageHandler("Handler4"));
    m_broker1.unregisterMessageHandler("Handler2");
    EXPECT_FALSE(m_broker1.hasRegisteredMessageHandler("Handler2"));
    EXPECT_TRUE(m_broker1.hasRegisteredMessageHandler("Handler3"));
}

TEST_F(CommandMessageBrokerIntegrationTest, ConnectAndDisconnect)
{
    Client     client(m_ioContext2);
    const auto address = CommandMessageBroker::createFullQualifiedAddress(
        m_brokerIds.at(0), CommandMessageBroker::Service::Responder);
    EXPECT_TRUE(client.connect(address));
    EXPECT_TRUE(client.disconnect());
}

TEST_F(CommandMessageBrokerIntegrationTest, RequestHandler)
{
    Client client(m_ioContext2);
    m_broker1.registerMessageHandler("Wonderful.Day", [&](const message::Command& command) {
        message::CommandResponse response;
        response.set_id(command.id());
        response.set_result(message::CommandResponse_Result_SUCCESS);
        response.set_response(command.parameters());
        return response;
    });
    EXPECT_TRUE(client.connect(CommandMessageBroker::createFullQualifiedAddress(
        m_brokerIds.at(0), CommandMessageBroker::Service::Responder)));
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

TEST_F(CommandMessageBrokerIntegrationTest, PostRequestProcessing)
{
    static const std::string requestId1  = "request-1";
    static const std::string requestId2  = "request-2";
    static const std::string responseId2 = "response-2";
    static const std::string responseId1 = "response-1";

    bool hasReceived = false;
    m_broker2.registerPostProcessHandler([&] {
        message::CommandResponse response;
        EXPECT_TRUE(m_broker2.send(createCommand(requestId1), m_brokerIds.at(0), response));
        EXPECT_EQ(response.response(), responseId1);
    });
    m_broker2.registerMessageHandler(requestId2, [&](const message::Command&) {
        message::CommandResponse response;
        response.set_response(responseId2);
        return response;
    });
    m_broker1.registerMessageHandler(requestId1, [&](const message::Command&) {
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

TEST_F(CommandMessageBrokerIntegrationTest, ForbitSendingDuringRequestProcessing)
{
    static const std::string requestId1  = "request-1";
    static const std::string requestId2  = "request-2";
    static const std::string responseId2 = "response-2";
    static const std::string responseId1 = "response-1";

    bool hasReceived = false;
    m_broker2.registerMessageHandler(requestId2, [&](const message::Command&) {
        message::CommandResponse response;
        EXPECT_FALSE(m_broker2.send(createCommand(requestId1), m_brokerIds.at(0), response));
        response.set_response(responseId2);
        return response;
    });
    m_broker1.registerMessageHandler(requestId1, [&](const message::Command&) {
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

TEST_F(CommandMessageBrokerIntegrationTest, NotifyAllHandlers)
{
    static const std::string notificationName = "hello";

    m_broker2.registerMessageHandler(notificationName, [&](const message::Command& command) {
        notifyReceivedMessage(command.name());
        return message::CommandResponse();
    });
    EXPECT_TRUE(m_broker2.subscribe(m_brokerIds[0], notificationName));
    m_broker3.registerMessageHandler(notificationName, [&](const message::Command& command) {
        notifyReceivedMessage(command.name());
        return message::CommandResponse();
    });
    EXPECT_TRUE(m_broker3.subscribe(m_brokerIds[0], notificationName));
    message::Command notification =
        createCommand(notificationName, 42, "", message::Command_Type_Notification);
    EXPECT_TRUE(m_broker1.notify(notification, notificationName));
    waitForReceivedMessages(2);
    EXPECT_EQ(m_messageReceiveQueue.front(), notificationName);
    m_messageReceiveQueue.pop();
    EXPECT_EQ(m_messageReceiveQueue.front(), notificationName);
}
