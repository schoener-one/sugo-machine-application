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

#include "Common/IConfigurationMock.hpp"
#include "Common/IOContext.hpp"
#include "Common/IProcessContextMock.hpp"
#include "Common/Logger.hpp"
#include "Common/ServiceLocator.hpp"
#include "MessageBroker/IMessageBrokerMock.hpp"
#include "ServiceGateway/ServiceGateway.hpp"

using ::testing::_;
using ::testing::Return;

using namespace sugo::common;
using namespace sugo::message_broker;
using namespace sugo::service_component;
using namespace sugo::service_gateway;

class ServiceGatewayTest : public ::testing::Test
{
protected:
    ServiceGatewayTest()
        : m_ioContext("ServiceGatewayTest"),
          m_serviceGateway(m_mockMessageBroker, m_mockProcessContext, m_serviceLocator, m_ioContext)
    {
    }

    static void SetUpTestCase()
    {
        Logger::init();
    }

    void SetUp() override
    {
        m_serviceLocator.add<IConfiguration>(m_mockConfiguration);
    }

    void TearDown() override
    {
    }

    ServiceLocator      m_serviceLocator;
    IConfigurationMock  m_mockConfiguration;
    IOContext           m_ioContext;
    IMessageBrokerMock  m_mockMessageBroker;
    IProcessContextMock m_mockProcessContext;
    ServiceGateway      m_serviceGateway;
};

TEST_F(ServiceGatewayTest, Start)
{
    // TODO no implemented yet!
    // EXPECT_CALL(m_mockProcessContext, start()).WillOnce(Return(true));
    // EXPECT_TRUE(m_serviceGateway.start());
}