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
#include <thread>

#include "Common/Logger.hpp"
#include "Common/ProcessContext.hpp"
#include "MessageBroker/IMessageBrokerMock.hpp"
#include "ServiceComponent/IStateMachineMock.hpp"
#include "ServiceComponent/StateMachine.hpp"
#include "ServiceComponent/StatedServiceComponent.hpp"

using ::testing::_;
using ::testing::Return;

namespace sugo::service_component
{
class StatedServiceComponentTestable : public StatedServiceComponent<test::State, test::Event>
{
public:
    using StatedServiceComponent<test::State, test::Event>::StatedServiceComponent;
};
}  // namespace sugo::service_component

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::message_broker;
using namespace sugo::common;

class StatedServiceComponentIntegrationTest : public ::testing::Test
{
protected:
    using Transition = StateMachine<test::State, test::Event>::Transition;

    StatedServiceComponentIntegrationTest()
        : m_stateMachine(test::State1,
                         {Transition(test::State::State1, test::State::State2, test::Event1),
                          Transition(test::State::State2, test::State::State1, test::Event2)}),
          m_processContext{"StatedServiceComponentIntegrationTest"},
          m_component(m_mockMessageBroker, m_subscriptionIds, m_stateMachine, m_processContext)
    {
    }

    ~StatedServiceComponentIntegrationTest() override
    {
    }

    static void SetUpTestCase()
    {
        Logger::init(Logger::Severity::trace);
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    IMessageBrokerMock                     m_mockMessageBroker;
    StateMachine<test::State, test::Event> m_stateMachine;
    IServiceComponent::NotificationIdList  m_subscriptionIds;
    common::ProcessContext                 m_processContext;
    StatedServiceComponentTestable         m_component;
};

TEST_F(StatedServiceComponentIntegrationTest, StartAndStop)
{
    EXPECT_CALL(m_mockMessageBroker, start()).WillOnce(Return(true));
    EXPECT_TRUE(m_component.start());
    EXPECT_CALL(m_mockMessageBroker, isRunning()).WillOnce(Return(true));
    EXPECT_TRUE(m_component.isRunning());
    EXPECT_CALL(m_mockMessageBroker, stop()).Times(1);
    m_component.stop();
    EXPECT_CALL(m_mockMessageBroker, isRunning()).WillOnce(Return(false));
    EXPECT_FALSE(m_component.isRunning());
}

TEST_F(StatedServiceComponentIntegrationTest, PushEvent)
{
    EXPECT_CALL(m_mockMessageBroker, start()).WillOnce(Return(true));
    EXPECT_TRUE(m_component.start());
    EXPECT_TRUE(m_stateMachine.push(test::Event1));
    EXPECT_EQ(test::State1, m_stateMachine.getCurrentState());
    EXPECT_CALL(m_mockMessageBroker, stop()).Times(1);
    m_component.stop();
}