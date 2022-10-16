///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2022-09-25
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>

#include "EventQueue.hpp"
#include "Logger.hpp"

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

using namespace sugo;

class EventQueueTest : public ::testing::Test
{
protected:
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

    EventQueue<Event> m_testQueue;
};

TEST_F(EventQueueTest, PushAndPull)
{
    Event       lastEvent = Event::One;
    std::thread t([&] {
        Logger::reinit();
        do
        {
            while (m_testQueue.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            lastEvent = m_testQueue.pull();
            LOG(info) << "Pulled event: " << lastEvent;
        } while (lastEvent != Event::Three);
    });

    LOG(info) << "Pushed event: " << Event::One;
    m_testQueue.push(Event::One);
    LOG(info) << "Pushed event: " << Event::Two;
    m_testQueue.push(Event::Two);
    LOG(info) << "Pushed event: " << Event::Three;
    m_testQueue.push(Event::Three);

    t.join();
    EXPECT_EQ(Event::Three, lastEvent);
}
