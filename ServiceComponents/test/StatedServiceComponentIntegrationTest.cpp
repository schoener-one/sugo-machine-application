///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ICommandMessageBrokerMock.hpp"
#include "IStateMachineMock.hpp"
#include "Logger.hpp"
#include "StateMachine.hpp"
#include "StatedServiceComponent.hpp"

#include <thread>

using ::testing::_;
using ::testing::Return;

namespace sugo
{
class StatedServiceComponentTestable : public StatedServiceComponent<test::State, test::Event>
{
public:
    using StatedServiceComponent<test::State, test::Event>::StatedServiceComponent;
    using StatedServiceComponent<test::State, test::Event>::processAllEvents;
};
}  // namespace sugo

using namespace sugo;
using namespace sugo::message;

class StatedServiceComponentIntegrationTest : public ::testing::Test
{
protected:
    using Transition = StateMachine<test::State, test::Event>::Transition;

    StatedServiceComponentIntegrationTest()
        : m_stateMachine(test::State1,
                         {Transition(test::State::State1, test::State::State2, test::Event1),
                          Transition(test::State::State2, test::State::State1, test::Event2)}),
          m_component(m_mockCommandMessageBroker, m_subscriptionIds, m_stateMachine)
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

    ICommandMessageBrokerMock              m_mockCommandMessageBroker;
    StateMachine<test::State, test::Event> m_stateMachine;
    IServiceComponent::NotificationIdList  m_subscriptionIds;
    StatedServiceComponentTestable         m_component;
};

TEST_F(StatedServiceComponentIntegrationTest, StartAndStop)
{
    EXPECT_CALL(m_mockCommandMessageBroker, start()).WillOnce(Return(true));
    EXPECT_CALL(m_mockCommandMessageBroker, registerPostProcessHandler(_)).Times(1);
    EXPECT_TRUE(m_component.start());
    EXPECT_CALL(m_mockCommandMessageBroker, isRunning()).WillOnce(Return(true));
    EXPECT_TRUE(m_component.isRunning());
    EXPECT_CALL(m_mockCommandMessageBroker, stop()).Times(1);
    m_component.stop();
    EXPECT_CALL(m_mockCommandMessageBroker, isRunning()).WillOnce(Return(false));
    EXPECT_FALSE(m_component.isRunning());
}

TEST_F(StatedServiceComponentIntegrationTest, PushEvent)
{
    EXPECT_CALL(m_mockCommandMessageBroker, start()).WillOnce(Return(true));
    EXPECT_CALL(m_mockCommandMessageBroker, registerPostProcessHandler(_)).Times(1);
    EXPECT_TRUE(m_component.start());
    EXPECT_TRUE(m_stateMachine.push(test::Event1));
    EXPECT_EQ(test::State1, m_stateMachine.getCurrentState());
    m_component.processAllEvents();
    EXPECT_EQ(test::State2, m_stateMachine.getCurrentState());
    EXPECT_CALL(m_mockCommandMessageBroker, stop()).Times(1);
    m_component.stop();
}