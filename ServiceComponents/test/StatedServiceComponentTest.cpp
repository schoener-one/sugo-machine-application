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
#include "ProcessContext.hpp"
#include "StatedServiceComponent.hpp"

class StatedServiceComponentTest;

namespace sugo
{
class StatedServiceComponentTestable : public StatedServiceComponent<test::State, test::Event>
{
public:
    StatedServiceComponentTestable(ICommandMessageBroker&                   messageBroker,
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

TEST_F(StatedServiceComponentTest, StatedServiceComponent_Send)
{
    const CommandId   commandId("test", "send-this");
    const std::string parameters("test-parameters");
    EXPECT_CALL(m_mockCommandMessageBroker, send(_, commandId.receiver, _))
        .WillOnce(::testing::Return(true));
    const message::CommandResponse response = m_component.send(commandId, parameters);
    EXPECT_EQ(response.result(), message::CommandResponse_Result_SUCCESS);
}

TEST_F(StatedServiceComponentTest, StatedServiceComponent_Notify)
{
    const CommandId   commandId("", "send-this");
    const std::string parameters("test-parameters");
    EXPECT_CALL(m_mockCommandMessageBroker, notify(_, _)).WillOnce(::testing::Return(true));
    const bool success = m_component.notify(commandId, parameters);
    EXPECT_TRUE(success);
}
