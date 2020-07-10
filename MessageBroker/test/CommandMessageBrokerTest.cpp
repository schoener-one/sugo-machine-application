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

#include <istream>
#include <ostream>
#include <thread>

#include <Command.pb.h>

#include "CommandMessageBroker.hpp"

#include "IOContextHelper.hpp"
#include "Logger.hpp"
#include "Requester.hpp"

using namespace moco;

class CommandMessageBrokerTest : public ::testing::Test, public IOContextHelper
{
protected:
    CommandMessageBrokerTest() {}

    ~CommandMessageBrokerTest() override {}

    static void SetUpTestCase() { Logger::init(Logger::Severity::error); }

    void SetUp() override {}

    void TearDown() override { stopIOContext(); }

    void sleepFor(std::chrono::seconds seconds) { std::this_thread::sleep_for(seconds); }
};

TEST_F(CommandMessageBrokerTest, CommandMessageBroker_RegisterHandler)
{
    CommandMessageBroker     cmb("TestBroker", {}, m_ioContext);
    message::CommandResponse dummy;
    cmb.registerHandler("Handler1", [&](const message::Command&) { return dummy; });
    cmb.registerHandler("Handler2", [&](const message::Command&) { return dummy; });
    cmb.registerHandler("Handler3", [&](const message::Command&) { return dummy; });
    EXPECT_TRUE(cmb.hasRegisteredHandler("Handler2"));
    EXPECT_FALSE(cmb.hasRegisteredHandler("Handler4"));
    cmb.unregisterHandler("Handler2");
    EXPECT_FALSE(cmb.hasRegisteredHandler("Handler2"));
    EXPECT_TRUE(cmb.hasRegisteredHandler("Handler3"));
}

TEST_F(CommandMessageBrokerTest, CommandMessageBroker_Connect)
{
    CommandMessageBroker cmb("TestBroker", {}, m_ioContext);
    Requester            requester(m_ioContext);
    startIOContext();
    sleepFor(std::chrono::seconds(1));
    EXPECT_TRUE(requester.connect(CommandMessageBroker::createAddress("TestBroker")));
}

TEST_F(CommandMessageBrokerTest, CommandMessageBroker_RequestHandler)
{
    const std::string    address = "test-address";
    CommandMessageBroker cmb(address, {}, m_ioContext);
    Requester            requester(m_ioContext);
    cmb.registerHandler("Wonderful.Day", [&](const message::Command& command) {
        message::CommandResponse dummy;
        dummy.set_id(command.id());
        dummy.set_result(message::CommandResponse_Result_SUCCESS);
        dummy.set_response(command.parameters());
        return dummy;
    });
    EXPECT_TRUE(cmb.start());
    startIOContext();
    EXPECT_TRUE(requester.connect(CommandMessageBroker::createAddress(address)));
    message::Command command;
    command.set_id(42);
    command.set_name("Wonderful.Day");
    command.set_parameters("{ \"param1\": 42 }");
    StreamBuffer sendBuf, receiveBuf;
    std::ostream out(&sendBuf);
    EXPECT_TRUE(command.SerializeToOstream(&out));
    EXPECT_TRUE(requester.send(sendBuf, receiveBuf));
    std::istream             in(&receiveBuf);
    message::CommandResponse response;
    EXPECT_TRUE(response.ParseFromIstream(&in));
    EXPECT_EQ(response.id(), 42);
    EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
    EXPECT_EQ(response.response(), "{ \"param1\": 42 }");
}

TEST_F(CommandMessageBrokerTest, CommandMessageBroker_NotifyAllHandlers)
{
    message::CommandResponse                   dummy;
    const CommandMessageBroker::ReceiverIdList receiverIdList = {"cmb1", "cmb2", "cmb3"};
    CommandMessageBroker                       cmb1("cmb1", receiverIdList, m_ioContext);
    CommandMessageBroker                       cmb2("cmb2", receiverIdList, m_ioContext);
    CommandMessageBroker                       cmb3("cmb3", receiverIdList, m_ioContext);
    bool                                       hasCmb2Received = false, hasCmb3Received = false;
    cmb2.registerHandler("hello", [&](const message::Command& command) {
        hasCmb2Received = (command.id() == 42);
        return dummy;
    });
    cmb3.registerHandler("hello", [&](const message::Command& command) {
        hasCmb3Received = (command.id() == 42);
        return dummy;
    });
    EXPECT_TRUE(cmb1.start());
    EXPECT_TRUE(cmb2.start());
    EXPECT_TRUE(cmb3.start());
    startIOContext();
    message::Command notification;
    notification.set_id(42);
    notification.set_name("hello");
    EXPECT_TRUE(cmb1.notify(notification));
    sleepFor(std::chrono::seconds(1));
    EXPECT_TRUE(hasCmb2Received);
    EXPECT_TRUE(hasCmb3Received);
}
