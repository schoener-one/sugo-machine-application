///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   25.01.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>

#include "Logger.hpp"
#include "Thread.hpp"
#include "Timer.hpp"

using namespace sugo;

class CommonTest : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        Logger::init();
    }
};

#ifdef ENABLE_REALTIMETHREAD_TEST
TEST_F(CommonTest, RealTimeThread)
{
    Thread          thread;
    std::thread::id id;

    thread.start([&] {
        id = std::this_thread::get_id();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    });
    const std::thread::id threadId = thread.getId();
    thread.join();

    EXPECT_EQ(threadId, id);
}
#endif

TEST_F(CommonTest, TimerStartStop)
{
    const std::chrono::milliseconds period(200);
    const std::chrono::milliseconds duration(1000);
    unsigned                        counts   = 0;
    std::chrono::time_point         stopTime = std::chrono::high_resolution_clock::now();
    Timer                           timer(
        period,
        [&counts, &stopTime] {
            counts++;
            stopTime = std::chrono::high_resolution_clock::now();
        },
        "TestTimer");
    auto startTime = std::chrono::high_resolution_clock::now();
    EXPECT_TRUE(timer.start());
    std::this_thread::sleep_for(duration);
    timer.stop();
    const unsigned expectedCounts = duration / period;
    EXPECT_EQ(expectedCounts, counts);
    const auto elapsedTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime);
    EXPECT_NEAR(elapsedTime.count(), duration.count(), 10);
}

TEST_F(CommonTest, TimeoutHandlerDelay)
{
    const std::chrono::milliseconds period(200);
    const std::chrono::milliseconds duration(1000);
    unsigned                        counts   = 0;
    std::chrono::time_point         stopTime = std::chrono::high_resolution_clock::now();
    Timer                           timer(
        period,
        [&counts, &stopTime, &period] {
            counts++;
            stopTime = std::chrono::high_resolution_clock::now();
            if (counts == 3)
            {
                std::this_thread::sleep_for(period);
            }
        },
        "TestTimer");
    auto startTime = std::chrono::high_resolution_clock::now();
    EXPECT_TRUE(timer.start());
    std::this_thread::sleep_for(duration);
    timer.stop();
    const unsigned expectedCounts = duration / period - 1;  // One less because of the delay!
    EXPECT_EQ(expectedCounts, counts);
    const auto elapsedTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime);
    EXPECT_NEAR(elapsedTime.count(), duration.count(), 10);
}