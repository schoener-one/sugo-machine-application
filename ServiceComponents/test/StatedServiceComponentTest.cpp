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

#include "CommandId.hpp"
#include "ICommandMessageBrokerMock.hpp"
#include "IStateMachineMock.hpp"
#include "Logger.hpp"

#include "StatedServiceComponent.hpp"

class StatedServiceComponentTest;

namespace sugo
{
class StatedServiceComponentTestable : public StatedServiceComponent<test::State, test::Event>
{
public:
    StatedServiceComponentTestable(ICommandMessageBroker&                   messageBroker,
                                   IStateMachine<test::State, test::Event>& stateMachine)
        : StatedServiceComponent<test::State, test::Event>(messageBroker, stateMachine)
    {
    }

    using ServiceComponent::send;
    using ServiceComponent::notify;
    friend class StatedServiceComponentTest;
};
}  // namespace sugo

using namespace sugo;

class StatedServiceComponentTest : public ::testing::Test
{
protected:
    StatedServiceComponentTest() : m_testComponent(m_commandMessageBrokerMock, m_stateMachineMock)
    {
    }

    ~StatedServiceComponentTest() override {}

    static void SetUpTestCase() { Logger::init(); }

    void SetUp() override {}

    void TearDown() override {}

    ICommandMessageBrokerMock      m_commandMessageBrokerMock;
    IStateMachineMock              m_stateMachineMock;
    StatedServiceComponentTestable m_testComponent;
};

TEST_F(StatedServiceComponentTest, CommandExecutionComponent_Send)
{
    const CommandId   commandId("test", "send-this");
    const std::string parameters("test-parameters");
    EXPECT_CALL(m_commandMessageBrokerMock, send(::testing::_, commandId.receiver, ::testing::_))
        .WillOnce(::testing::Return(true));
    const message::CommandResponse response = m_testComponent.send(commandId, parameters);
    EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
}

TEST_F(StatedServiceComponentTest, CommandExecutionComponent_Notify)
{
    const CommandId   commandId("", "send-this");
    const std::string parameters("test-parameters");
    EXPECT_CALL(m_commandMessageBrokerMock,
    notify(::testing::_)).WillOnce(::testing::Return(true)); const bool success =
    m_testComponent.notify(commandId, parameters); EXPECT_TRUE(success);
}
