//////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-08-03
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "FilamentCoilMotor.hpp"
#include "ICommandMessageBrokerMock.hpp"
#include "IHardwareAbstractionLayerMock.hpp"
#include "IStepperMotorControlMock.hpp"
#include "IStepperMotorMock.hpp"
#include "Logger.hpp"
#include "ServiceLocator.hpp"

class MachineServiceComponentsTest;

using namespace sugo;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

class TestFilamentCoilMotor : public FilamentCoilMotor
{
    FRIEND_TEST(MachineServiceComponentsTest, FilamentCoilMotor_Start);

public:
    using FilamentCoilMotor::FilamentCoilMotor;
};

class MachineServiceComponentsTest : public ::testing::Test
{
protected:
    MachineServiceComponentsTest()
        : m_mockStepperMotorControl(new NiceMock<IStepperMotorControlMock>()),
          m_mockStepperMotor(new IStepperMotorMock())
    {
    }

    ~MachineServiceComponentsTest() override
    {
    }

    void SetUp() override
    {
        m_stepperMotorControllerMap.emplace(StepperMotorControlName, m_mockStepperMotorControl);
        m_stepperMotorMap.emplace(StepperMotorName, m_mockStepperMotor);
        m_serviceLocator.add<IHardwareAbstractionLayer>(m_mockHardwareAbstractionLayer);
        ON_CALL(m_mockHardwareAbstractionLayer, getStepperMotorControllerMap())
            .WillByDefault(ReturnRef(m_stepperMotorControllerMap));
        ON_CALL(*m_mockStepperMotorControl, getStepperMotorMap())
            .WillByDefault(ReturnRef(m_stepperMotorMap));
    }

    void TearDown() override
    {
    }

    static void SetUpTestCase()
    {
        Logger::init();
    }

    inline static const std::string StepperMotorName{"coiler"};
    inline static const std::string StepperMotorControlName{"stepper-motor-control"};

    IHardwareAbstractionLayer::StepperMotorControllerMap m_stepperMotorControllerMap;
    IStepperMotorControl::StepperMotorMap                m_stepperMotorMap;

    std::shared_ptr<IStepperMotorControlMock> m_mockStepperMotorControl;
    std::shared_ptr<IStepperMotorMock>        m_mockStepperMotor;

    NiceMock<IHardwareAbstractionLayerMock> m_mockHardwareAbstractionLayer;
    NiceMock<ICommandMessageBrokerMock>     m_mockCommandMessageBroker;
    ServiceLocator                          m_serviceLocator;
};

TEST_F(MachineServiceComponentsTest, FilamentCoilMotor_Start)
{
    TestFilamentCoilMotor comp(m_mockCommandMessageBroker, m_serviceLocator);

    EXPECT_CALL(m_mockCommandMessageBroker, start()).WillOnce(Return(true));
    EXPECT_TRUE(comp.start());

    EXPECT_CALL(*m_mockStepperMotor, reset()).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotor, getMaxSpeed())
        .WillOnce(Return(IStepperMotor::Speed(100, Unit::Rpm)));
    message::Command command;
    command.set_id(4);
    auto response = comp.onCommandSwitchOn(command);
    EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
    EXPECT_EQ(comp.getCurrentState(), FilamentCoilMotor::State::Stopped);

    EXPECT_CALL(*m_mockStepperMotor, rotate(IStepperMotor::Direction::Forward))
        .WillOnce(Return(true));
    response = comp.onCommandStartMotor(command);
    EXPECT_EQ(response.id(), 4);
    EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
    EXPECT_EQ(comp.getCurrentState(), FilamentCoilMotor::State::Running);
}
