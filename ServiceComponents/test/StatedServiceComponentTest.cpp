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
#include "ProcessContext.hpp"
#include "StatedServiceComponent.hpp"

class StatedServiceComponentTest;

namespace sugo
{
class StatedServiceComponentTestable : public StatedServiceComponent<test::State, test::Event>
{
public:
    StatedServiceComponentTestable(message::ICommandMessageBroker&          messageBroker,
                                   IStateMachine<test::State, test::Event>& stateMachine)
        : StatedServiceComponent<test::State, test::Event>(messageBroker, {}, stateMachine)
    {
    }

    using ServiceComponent::notify;
    using ServiceComponent::send;
    friend class StatedServiceComponentTest;
};
}  // namespace sugo

using ::testing::_;

using namespace sugo;
using namespace sugo::message;

class StatedServiceComponentTest : public ::testing::Test
{
protected:
    StatedServiceComponentTest() : m_component(m_mockCommandMessageBroker, m_mockStateMachine)
    {
    }

    ~StatedServiceComponentTest() override
    {
    }

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

    ICommandMessageBrokerMock      m_mockCommandMessageBroker;
    IStateMachineMock              m_mockStateMachine;
    StatedServiceComponentTestable m_component;
};

TEST_F(StatedServiceComponentTest, Send)
{
    const CommandId commandId("test", "send-this");
    EXPECT_CALL(m_mockCommandMessageBroker, send(_, commandId.getReceiverAddress(), _))
        .WillOnce(::testing::Return(true));
    const message::CommandResponse response = m_component.send(commandId, "test-parameters");
    EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
}

TEST_F(StatedServiceComponentTest, Notify)
{
    const NotificationId notificationId("", "send-this");
    EXPECT_CALL(m_mockCommandMessageBroker, notify(_, _)).WillOnce(::testing::Return(true));
    const bool success = m_component.notify(notificationId, "test-parameters");
    EXPECT_TRUE(success);
}
