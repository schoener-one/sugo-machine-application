/*
 * CoffeeAutomatAppTest.cpp
 *
 *  Created on: 02.01.2020
 *      Author: denis
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <thread>

#include "CircularMotionController.hpp"
#include "CommandMessageBroker.hpp"
#include "ICupRotationTray.hpp"

#include "CoffeeAutomatController.hpp"
#include "IMessageBroker.hpp"
#include "IOContextHelper.hpp"
#include "Logger.hpp"

namespace moco
{
class IStepperMotorMock : public IStepperMotor
{
public:
    MOCK_METHOD0(reset, void());
    MOCK_METHOD3(runSteps, unsigned(unsigned steps, Direction direction, RunGuard runGuard));
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
}  // namespace moco

using namespace moco;

class CoffeeAutomatSmokeTest : public ::testing::Test, public IOContextHelper
{
protected:
    CoffeeAutomatSmokeTest() {}

    ~CoffeeAutomatSmokeTest() override {}

    static void SetUpTestCase() { Logger::init(); }

    void SetUp() override {}

    void TearDown() override { stopIOContext(); }

    void startIoService() {}
};

TEST_F(CoffeeAutomatSmokeTest, CoffeeAutomat_CommandResponse)
{
    CommandMessageBroker    messageBroker(ICoffeeAutomatController::Command::ReceiverId, {},
                                       m_ioContext);
    CoffeeAutomatController coffeeAutomatController(messageBroker);
    EXPECT_TRUE(messageBroker.start());
    startIOContext();
    message::Command command;
    command.set_name(ICoffeeAutomatController::Command::SwitchOff.command);
    message::CommandResponse response;
    EXPECT_TRUE(
        messageBroker.send(command, ICoffeeAutomatController::Command::ReceiverId, response));
    EXPECT_EQ(message::CommandResponse::INVALID_STATE, response.result());

    messageBroker.registerHandler(ICoffeeAutomatController::Command::SwitchOff.command,
                                  [&](const message::Command&) -> message::CommandResponse {
                                      message::CommandResponse response;
                                      response.set_result(message::CommandResponse_Result_SUCCESS);
                                      return response;
                                  });
    EXPECT_TRUE(
        messageBroker.send(command, ICoffeeAutomatController::Command::ReceiverId, response));
    EXPECT_EQ(message::CommandResponse::SUCCESS, response.result());
}

TEST_F(CoffeeAutomatSmokeTest, CircularMotionController_ResetNoMotion)
{
    IStepperMotorMock                             stepperMotorMock;
    CircularMotionController::PositionSwitchArray positionSwitchArray = {
        std::make_unique<IPositionSwitchMock>(), std::make_unique<IPositionSwitchMock>(),
        std::make_unique<IPositionSwitchMock>()};
    CircularMotionController circularMotionController(stepperMotorMock, positionSwitchArray);
    ::testing::InSequence    s;
    EXPECT_CALL(stepperMotorMock, reset).Times(1);
    IPositionSwitchMock* mock = static_cast<IPositionSwitchMock*>(positionSwitchArray[0].get());
    EXPECT_CALL(*mock, isOpen).WillRepeatedly(::testing::Return(false));
    EXPECT_TRUE(circularMotionController.reset());
}

TEST_F(CoffeeAutomatSmokeTest, CircularMotionController_ResetWithMotion)
{
    IStepperMotorMock                             stepperMotorMock;
    CircularMotionController::PositionSwitchArray positionSwitchArray = {
        std::make_unique<IPositionSwitchMock>(), std::make_unique<IPositionSwitchMock>(),
        std::make_unique<IPositionSwitchMock>()};
    CircularMotionController circularMotionController(stepperMotorMock, positionSwitchArray);
    ::testing::InSequence    s;
    EXPECT_CALL(stepperMotorMock, reset).Times(1);
    EXPECT_TRUE(circularMotionController.reset());
}
