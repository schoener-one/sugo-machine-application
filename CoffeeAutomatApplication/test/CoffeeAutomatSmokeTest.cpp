/*
 * CoffeeAutomatAppTest.cpp
 *
 *  Created on: 02.01.2020
 *      Author: denis
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <memory>

#include "ICupRotationTray.hpp"
#include "IMessageBroker.hpp"
#include "CoffeeAutomat.hpp"
#include "ICoffeeAutomat.hpp"
#include "CircularMotionController.hpp"
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

class IStepperMotorMock : public IStepperMotor
{
public:
	MOCK_METHOD0(reset, void());
	MOCK_METHOD3(runSteps, unsigned(unsigned steps, Direction direction,
			RunGuard runGuard));
	MOCK_METHOD0(stop, void());
	MOCK_METHOD0(runSelfTest, bool());
	MOCK_CONST_METHOD0(getStepsPerRound, unsigned());
};

class IPositionSwitchMock : public IPositionSwitch
{
public:
	MOCK_CONST_METHOD0(isOpen, bool());
	MOCK_CONST_METHOD1(waitForChange, bool(int));
};
}

using namespace moco;

class CoffeeAutomatSmokeTest : public ::testing::Test
{
protected:
	CoffeeAutomatSmokeTest() {}

	~CoffeeAutomatSmokeTest() override {}

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

TEST_F(CoffeeAutomatSmokeTest, CoffeeAutomat_CommandResponse)
{
	CommandMessageBroker messageBroker;
	CoffeeAutomatImpl coffeeAutomat(messageBroker);
	message::Command command;
	command.set_name(command::CoffeeAutomatSwitchOff);
	const message::CommandResponse response1 = messageBroker.send(command, command::CoffeeAutomatSwitchOff);
	EXPECT_EQ(message::CommandResponse::INVALID_STATE, response1.result());

	messageBroker.registerReceiver(command::CupRotationTraySwitchOn,
		[&](const message::Command&) -> message::CommandResponse{
		message::CommandResponse response;
		response.set_result(message::CommandResponse_Result_SUCCESS);
		return response;
	});
	messageBroker.registerReceiver(command::CupRotationTrayReset,
		[&](const message::Command&) -> message::CommandResponse{
		message::CommandResponse response;
		response.set_result(message::CommandResponse_Result_SUCCESS);
		return response;
	});
	const message::CommandResponse response2 = messageBroker.send(command, command::CoffeeAutomatSwitchOn);
	EXPECT_EQ(message::CommandResponse::SUCCESS, response2.result());
}

TEST_F(CoffeeAutomatSmokeTest, CircularMotionController_ResetNoMotion)
{
	IStepperMotorMock stepperMotorMock;
	CircularMotionController::PositionSwitchArray positionSwitchArray =
	{
		std::make_shared<IPositionSwitchMock>(),
		std::make_shared<IPositionSwitchMock>(),
		std::make_shared<IPositionSwitchMock>()
	};
	CircularMotionController circularMotionController(stepperMotorMock,
			positionSwitchArray);
	::testing::InSequence s;
	EXPECT_CALL(stepperMotorMock, reset).Times(1);
	EXPECT_CALL(*std::static_pointer_cast<IPositionSwitchMock>(positionSwitchArray[0]),
			isOpen()).WillRepeatedly(::testing::Return(false));
	EXPECT_TRUE(circularMotionController.reset());
}

TEST_F(CoffeeAutomatSmokeTest, CircularMotionController_ResetWithMotion)
{
	IStepperMotorMock stepperMotorMock;
	CircularMotionController::PositionSwitchArray positionSwitchArray =
	{
		std::make_shared<IPositionSwitchMock>(),
		std::make_shared<IPositionSwitchMock>(),
		std::make_shared<IPositionSwitchMock>()
	};
	CircularMotionController circularMotionController(stepperMotorMock,
			positionSwitchArray);
	::testing::InSequence s;
	EXPECT_CALL(stepperMotorMock, reset).Times(1);
	EXPECT_TRUE(circularMotionController.reset());
}
