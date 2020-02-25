/*
 * MessageBrokerTest.cpp
 *
 *  Created on: 11.03.2020
 *      Author: denis
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <memory>

#include "ICommandMessageBroker.hpp"
#include "IMessageBroker.hpp"
#include "Logger.hpp"

namespace moco
{
class ICommandMessageBrokerMock : public IMessageBroker<message::Command, message::CommandResponse, std::string>
{
public:
	using typename IMessageBroker<message::Command, message::CommandResponse, std::string>::Receiver;

	MOCK_CONST_METHOD1(hasReceiver, bool(const std::string&));
	MOCK_CONST_METHOD2(send, message::CommandResponse(const message::Command&, const std::string&));
	MOCK_METHOD2(registerReceiver, void(const std::string&, Receiver&));
	MOCK_METHOD2(registerReceiver, void(const std::string&, Receiver&&));
	MOCK_METHOD1(unregisterReceiver, void(const std::string&));
};
}

using namespace moco;

class MessageBrokerTest : public ::testing::Test
{
protected:
	MessageBrokerTest() {}

	~MessageBrokerTest() override {}

	static void SetUpTestCase()
	{
		Logger::init();
	}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(MessageBrokerTest, Test_CommandMessageBroker)
{
	CommandMessageBroker messageBroker;
	EXPECT_FALSE(messageBroker.hasReceiver("Id1"));
	EXPECT_FALSE(messageBroker.hasReceiver("Id2"));
	messageBroker.registerReceiver("Id1", [&](const message::Command&) -> message::CommandResponse {
		message::CommandResponse response;
		response.set_response("Response1");
		return response;
	});
	EXPECT_TRUE(messageBroker.hasReceiver("Id1"));
	messageBroker.registerReceiver("Id2", [&](const message::Command&) -> message::CommandResponse {
		message::CommandResponse response;
		response.set_response("Response2");
		return response;
	});
	EXPECT_TRUE(messageBroker.hasReceiver("Id2"));
	message::Command command;
	command.set_name("Command1");
	message::CommandResponse response1 = messageBroker.send(command, "Id1");
	EXPECT_EQ("Response1", response1.response());
	command.set_name("Command2");
	message::CommandResponse response2 = messageBroker.send(command, "Id2");
	EXPECT_EQ("Response2", response2.response());
	messageBroker.unregisterReceiver("Id1");
	messageBroker.unregisterReceiver("Id2");
	EXPECT_FALSE(messageBroker.hasReceiver("Id1"));
	EXPECT_FALSE(messageBroker.hasReceiver("Id2"));
}

