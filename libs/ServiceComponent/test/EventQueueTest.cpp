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
#include "ServiceComponent/EventQueue.hpp"

namespace
{
enum Event
{
    One = 1,
    Two,
    Three
};

inline std::ostream& operator<<(std::ostream& ostr, const Event& events)
{
    switch (events)
    {
        case Event::One:
            ostr << "One";
            break;
        case Event::Two:
            ostr << "Two";
            break;
        case Event::Three:
            ostr << "Three";
            break;
        default:
            ostr << "Unknown";
            break;
    }
    return ostr;
}
}  // namespace

using namespace sugo::service_component;
using namespace sugo;

class EventQueueTest : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        common::Logger::init();
    }

    EventQueue<Event> m_testQueue;
};

TEST_F(EventQueueTest, PushAndPull)
{
    Event       lastEvent = Event::One;
    std::thread t([&] {
        common::Logger::reinit();
        while (m_testQueue.pull(lastEvent))
        {
            LOG(info) << "Pulled event: " << lastEvent;
        }
    });

    LOG(info) << "Pushed event: " << Event::One;
    m_testQueue.push(Event::One);
    LOG(info) << "Pushed event: " << Event::Two;
    m_testQueue.push(Event::Two);
    LOG(info) << "Pushed event: " << Event::Three;
    m_testQueue.push(Event::Three);

    // Wait until all events processed!
    while (lastEvent != Event::Three)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    m_testQueue.reset();
    t.join();
    EXPECT_EQ(Event::Three, lastEvent);
}
