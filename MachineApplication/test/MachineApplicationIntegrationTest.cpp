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
#include <boost/lexical_cast>

#include "AsioContext.hpp"
#include "CommandId.hpp"
#include "CommandMessageBroker.hpp"
#include "IHardwareAbstractionLayerMock.hpp"
#include "IStepperMotorControlMock.hpp"
#include "IStepperMotorMock.hpp"
#include "Logger.hpp"
#include "ServiceComponentsExecutionGroup.hpp"
#include "ServiceLocator.hpp"
#include "Thread.hpp"

#include "IMachineControl.hpp"

class MachineApplicationIntegrationTest;

using namespace sugo;
using namespace hal;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

class MachineApplicationIntegrationTest : public ::testing::Test
{
protected:
    inline static const std::string ReceiverId{"MachineApplicationIntegrationTest"};

    MachineApplicationIntegrationTest()
        : m_mockStepperMotorControl(new NiceMock<IStepperMotorControlMock>()),
          m_mockStepperMotor(new IStepperMotorMock()),
          m_broker(ReceiverId, m_ioContext)
    {
    }

    ~MachineApplicationIntegrationTest() override
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

        m_runner.start([&] { EXPECT_TRUE(m_components.start(m_serviceLocator)); });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    void TearDown() override
    {
        m_runner.join();
    }

    static void SetUpTestCase()
    {
        Logger::init();
    }

    message::CommandResponse send(const CommandId& commandId)
    {
        message::Command command;
        command.set_id(m_idCount);
        command.set_name(commandId.command);
        message::CommandResponse response;
        EXPECT_TRUE(m_broker.send(command, commandId.receiver, response));
        EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
        EXPECT_EQ(response.id(), m_idCount);
        m_idCount++;
        return response;
    }

    template <class CompT>
    void expectState(const CompT::State state)
    {
        auto response = send(CompT::CommandGetState);
        auto json = Json::parse(response.response())
        EXPECT_FALSE(json.at("state").empty());
        EXPECT_EQ(static_cast<unsigned>(state), json.at("state").get<unsigned>())
    }

    inline static const std::string StepperMotorName{"coiler"};
    inline static const std::string StepperMotorControlName{"stepper-motor-control"};

    IHardwareAbstractionLayer::StepperMotorControllerMap m_stepperMotorControllerMap;
    IStepperMotorControl::StepperMotorMap                m_stepperMotorMap;

    std::shared_ptr<IStepperMotorControlMock> m_mockStepperMotorControl;
    std::shared_ptr<IStepperMotorMock>        m_mockStepperMotor;

    NiceMock<IHardwareAbstractionLayerMock> m_mockHardwareAbstractionLayer;
    AsioContext                             m_ioContext;
    CommandMessageBroker                    m_broker;
    ServiceLocator                          m_serviceLocator;
    ServiceComponentsExecutionGroup         m_components;
    Thread                                  m_runner;
    unsigned                                m_idCount = 0;
};

TEST_F(MachineApplicationIntegrationTest, StartExecutionGroup)
{
    // TODO not yet implemented!
    // EXPECT_CALL(*m_mockStepperMotor, reset()).WillOnce(Return(true));
    // EXPECT_CALL(*m_mockStepperMotor, getMaxSpeed());
    // send(IMachineControl::CommandSwitchOn);
    // send(IMachineControl::CommandGetState);
    // expectState<>(IMachineControl::State::Running);
}
