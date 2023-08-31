///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "Common/IConfigurationMock.hpp"
#include "Common/IProcessContextMock.hpp"
#include "Common/Logger.hpp"
#include "Common/ServiceLocator.hpp"
#include "HardwareAbstractionLayer/IHardwareAbstractionLayerMock.hpp"
#include "HardwareAbstractionLayer/IStepperMotorControlMock.hpp"
#include "HardwareAbstractionLayer/IStepperMotorMock.hpp"
#include "MachineServiceComponent/Protocol.hpp"
#include "MachineServiceComponentTest.hpp"
#include "MachineServiceComponentTest/MachineConfiguration.hpp"
#include "MessageBroker/IMessageBrokerMock.hpp"

using namespace sugo;
using namespace sugo::common;
using namespace sugo::hal;
using namespace sugo::machine_service_component;
using namespace sugo::message_broker;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

namespace sugo::test
{
class MachineServiceComponentTest : public ::testing::Test
{
protected:
    MachineServiceComponentTest()
        : m_mockStepperMotorControl(new NiceMock<IStepperMotorControlMock>()),
          m_mockStepperMotor(new IStepperMotorMock())
    {
    }

    void SetUp() override
    {
        m_stepperMotorControllerMap.emplace(StepperMotorControlName, m_mockStepperMotorControl);
        m_stepperMotorMap.emplace(StepperMotorName, m_mockStepperMotor);
        m_serviceLocator.add<IHardwareAbstractionLayer>(m_mockHardwareAbstractionLayer);
        m_serviceLocator.add<common::IConfiguration>(m_mockConfiguration);
        m_machineConfig.prepareOptions(m_mockConfiguration);
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
        common::Logger::init();
    }

    template <class StateMachineT>
    static void processAllEvents(StateMachineT& stateMachine)
    {
        while (!stateMachine.getEventQueue().empty())
        {
            EXPECT_TRUE(stateMachine.processNextEvent());
        }
    }

    inline static const std::string StepperMotorName{"coiler"};
    inline static const std::string StepperMotorControlName{hal::id::StepperMotorControl};

    IHardwareAbstractionLayer::StepperMotorControllerMap m_stepperMotorControllerMap;
    IStepperMotorControl::StepperMotorMap                m_stepperMotorMap;

    std::shared_ptr<IStepperMotorControlMock> m_mockStepperMotorControl;
    std::shared_ptr<IStepperMotorMock>        m_mockStepperMotor;

    NiceMock<IHardwareAbstractionLayerMock> m_mockHardwareAbstractionLayer;
    NiceMock<IMessageBrokerMock>            m_mockRequestMessageBroker;
    NiceMock<IConfigurationMock>            m_mockConfiguration;
    NiceMock<IProcessContextMock>           m_mockProcessContext;
    test::MachineConfiguration              m_machineConfig;
    common::ServiceLocator                  m_serviceLocator;
};

TEST_F(MachineServiceComponentTest, StartFilamentCoilMotor)
{
    EXPECT_CALL(*m_mockStepperMotor, getMaxSpeed())
        .WillOnce(Return(IStepperMotor::Speed(100, Unit::Rpm)));
    EXPECT_CALL(m_mockProcessContext, start()).WillOnce(Return(true));
    FilamentCoilMotorTestable filamentCoilMotor(m_mockRequestMessageBroker, m_mockProcessContext,
                                                m_serviceLocator);

    EXPECT_CALL(m_mockRequestMessageBroker, start()).WillOnce(Return(true));
    EXPECT_TRUE(filamentCoilMotor.start());

    EXPECT_CALL(*m_mockStepperMotor, reset()).WillOnce(Return(true));
    message_broker::Message request;
    request.setId(4);
    auto response = filamentCoilMotor.onRequestSwitchOn(request);
    EXPECT_EQ(response.getResult(), message_broker::ResponseMessage::Result::Success);
    processAllEvents(filamentCoilMotor);
    EXPECT_EQ(filamentCoilMotor.getCurrentState(), FilamentCoilMotor::State::Stopped);

    // Set motor speed
    const common::Json parameters(
        {{machine_service_component::id::Speed, test::MachineConfiguration::MotorSpeedDefault}});
    request.setPayload(parameters.dump());
    EXPECT_CALL(*m_mockStepperMotor, setSpeed(IStepperMotor::Speed{
                                         test::MachineConfiguration::MotorSpeedDefault, Unit::Rpm}))
        .WillOnce(Return(true));
    response = filamentCoilMotor.onRequestSetMotorSpeed(request);
    EXPECT_EQ(response.getResult(), message_broker::ResponseMessage::Result::Success);

    EXPECT_CALL(*m_mockStepperMotor, setSpeed(IStepperMotor::Speed{
                                         test::MachineConfiguration::MotorSpeedDefault, Unit::Rpm}))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotor, rotate()).WillOnce(Return(true));
    response = filamentCoilMotor.onRequestStartMotor(request);
    processAllEvents(filamentCoilMotor);
    EXPECT_EQ(response.getId(), 4);
    EXPECT_EQ(response.getResult(), message_broker::ResponseMessage::Result::Success);
    EXPECT_EQ(filamentCoilMotor.getCurrentState(), FilamentCoilMotor::State::Running);
}
}  // namespace sugo::test