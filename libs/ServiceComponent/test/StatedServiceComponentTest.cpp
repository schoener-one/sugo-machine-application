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

#include "Common/IProcessContextMock.hpp"
#include "Common/Logger.hpp"
#include "MessageBroker/IMessageBrokerMock.hpp"
#include "ServiceComponent/IStateMachineMock.hpp"
#include "ServiceComponent/StatedServiceComponent.hpp"

class StatedServiceComponentTest;

namespace sugo::service_component
{
class StatedServiceComponentTestable : public StatedServiceComponent<test::State, test::Event>
{
public:
    StatedServiceComponentTestable(message_broker::IMessageBroker&          messageBroker,
                                   IStateMachine<test::State, test::Event>& stateMachine,
                                   common::IProcessContext&                 processContext)
        : StatedServiceComponent<test::State, test::Event>(messageBroker, {}, stateMachine,
                                                           processContext)
    {
    }

    using ServiceComponent::notify;
    using ServiceComponent::send;
    friend class StatedServiceComponentTest;
};
}  // namespace sugo::service_component

using ::testing::_;

using namespace sugo::common;
using namespace sugo::message_broker;
using namespace sugo::service_component;

class StatedServiceComponentTest : public ::testing::Test
{
protected:
    StatedServiceComponentTest()
        : m_component(m_mockMessageBroker, m_mockStateMachine, m_mockProcessContext)
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

    IProcessContextMock            m_mockProcessContext;
    IMessageBrokerMock             m_mockMessageBroker;
    IStateMachineMock              m_mockStateMachine;
    StatedServiceComponentTestable m_component;
};

TEST_F(StatedServiceComponentTest, Send)
{
    const RequestId requestId(2, "address", "topic");
    EXPECT_CALL(m_mockMessageBroker, send(_, requestId.getAddress(), _))
        .WillOnce(::testing::Return(true));
    ResponseMessage response{};
    EXPECT_TRUE(m_component.send(requestId, "test-parameters", response));
    EXPECT_EQ(response.getResult(), ResponseMessage::Result::Success);
}

TEST_F(StatedServiceComponentTest, Notify)
{
    const NotificationId notificationId(2, "address", "topic");
    EXPECT_CALL(m_mockMessageBroker, notify(_, _)).WillOnce(::testing::Return(true));
    const bool success = m_component.notify(notificationId, "test-parameters");
    EXPECT_TRUE(success);
}
