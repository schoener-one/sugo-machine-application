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
#include <tuple>

#include "Common/IOContext.hpp"
#include "Common/IProcessContext.hpp"
#include "Common/Logger.hpp"
#include "ServiceComponent/ExecutionBundle.hpp"
#include "ServiceComponent/ExecutionGroup.hpp"
#include "ServiceComponent/ServiceComponent.hpp"

#include "Common/IOContext.hpp"
#include "MessageBroker/MessageBroker.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::message_broker;

using ::testing::StrEq;

namespace sugo::test
{
const IServiceComponent::NotificationIdList s_notificationIdList = {};

template <const char* IdentifierT>
class GenericServiceComponent : public ServiceComponent
{
public:
    GenericServiceComponent(message_broker::IMessageBroker& messageBroker, common::IProcessContext&,
                            bool& constructed, bool startSucceeds)
        : ServiceComponent(messageBroker, s_notificationIdList),
          m_constructed(constructed),
          m_startSucceeds(startSucceeds)
    {
        m_constructed = true;
    }

    ~GenericServiceComponent()
    {
        m_constructed = false;
    }
    static constexpr const char* Identifier = IdentifierT;

    bool start() override
    {
        return m_startSucceeds;
    }

private:
    bool& m_constructed;
    bool  m_startSucceeds;
};

static const char IdentifierA[] = "ServiceComponentA";
static const char IdentifierB[] = "ServiceComponentB";
static const char IdentifierC[] = "ServiceComponentC";
using ServiceComponentA         = GenericServiceComponent<IdentifierA>;
using ServiceComponentB         = GenericServiceComponent<IdentifierB>;
using ServiceComponentC         = GenericServiceComponent<IdentifierC>;
using ExecutionBundleA          = ExecutionBundle<test::ServiceComponentA, bool&, bool>;
using ExecutionBundleB          = ExecutionBundle<test::ServiceComponentB, bool&, bool>;
using ExecutionBundleC          = ExecutionBundle<test::ServiceComponentC, bool&, bool>;

// const auto& getById(const char* id)
// {

// }
}  // namespace sugo::test

class ExecutionGroupTest : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        common::Logger::init();
    }
};

TEST_F(ExecutionGroupTest, CreateExecutionGroup)
{
    bool serviceAConstructed = false, serviceBConstructed = false, serviceCConstructed = false;
    {
        ExecutionGroup executionGroup{test::ExecutionBundleA{serviceAConstructed, true},
                                      test::ExecutionBundleB{serviceBConstructed, true},
                                      test::ExecutionBundleC{serviceCConstructed, true}};

        EXPECT_TRUE(serviceAConstructed);
        EXPECT_TRUE(serviceBConstructed);
        EXPECT_TRUE(serviceCConstructed);
    }
    EXPECT_FALSE(serviceAConstructed);
    EXPECT_FALSE(serviceBConstructed);
    EXPECT_FALSE(serviceCConstructed);
}

TEST_F(ExecutionGroupTest, GetExecutionBundle)
{
    bool serviceAConstructed = false, serviceBConstructed = false, serviceCConstructed = false;
    ExecutionGroup executionGroup{test::ExecutionBundleA{serviceAConstructed, true},
                                  test::ExecutionBundleB{serviceBConstructed, true},
                                  test::ExecutionBundleC{serviceCConstructed, true}};
    const auto&    bundle = executionGroup.get<test::ExecutionBundleB>();
    EXPECT_STREQ(bundle.getId().c_str(), test::IdentifierB);
}

TEST_F(ExecutionGroupTest, StartStopExecutionGroup)
{
    bool           serviceAConstructed = false, serviceBConstructed = false;
    ExecutionGroup executionGroup{test::ExecutionBundleA{serviceAConstructed, true},
                                  test::ExecutionBundleB{serviceBConstructed, true}};

    EXPECT_TRUE(executionGroup.start());
    executionGroup.stop();
    EXPECT_TRUE(serviceAConstructed);
    EXPECT_TRUE(serviceBConstructed);
}

TEST_F(ExecutionGroupTest, StartExecutionGroupFails)
{
    bool serviceAConstructed = false, serviceBConstructed = false, serviceCConstructed = false;
    ExecutionGroup executionGroup{
        test::ExecutionBundleA{serviceAConstructed, true},
        test::ExecutionBundleB{serviceBConstructed, false},  // Component start fails
        test::ExecutionBundleC{serviceCConstructed, true}};
    EXPECT_FALSE(executionGroup.start());
}
