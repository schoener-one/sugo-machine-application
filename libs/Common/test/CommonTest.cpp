///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2023
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
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>

#include "Common/Ios.hpp"
#include "Common/Logger.hpp"
#include "Common/StringTokenizer.hpp"
#include "Common/Thread.hpp"
#include "Common/Timer.hpp"
#include "Common/Types.hpp"

namespace sugo::test
{
inline static const std::string TestId{"testid"};
}

using namespace sugo::common;
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
    const std::chrono::milliseconds       period(200);
    const std::chrono::milliseconds       duration(1000 + period.count() - 1);
    unsigned                              counts   = 0;
    std::chrono::time_point<Timer::Clock> stopTime = std::chrono::high_resolution_clock::now();
    Timer                                 timer(
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
    EXPECT_NEAR(elapsedTime.count(), duration.count(), period.count() - 1);
}

TEST_F(CommonTest, TimeoutHandlerDelay)
{
    const std::chrono::milliseconds       period(200);
    const std::chrono::milliseconds       duration(1000);
    unsigned                              counts   = 0;
    std::chrono::time_point<Timer::Clock> stopTime = std::chrono::high_resolution_clock::now();
    Timer                                 timer(
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

TEST_F(CommonTest, StringTokenizer)
{
    const std::string s1{"That is a"}, s2{" test string"}, s3{" which is separated by points"};
    const std::string testString{s1 + "." + s2 + "." + s3};

    StringTokenizer::Tokens tokens = StringTokenizer::split<'.'>(testString);
    EXPECT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens.at(0), s1);
    EXPECT_EQ(tokens.at(1), s2);
    EXPECT_EQ(tokens.at(2), s3);
}

TEST_F(CommonTest, OutputStreamHex)
{
    std::ostringstream oss;
    int32_t            int32Number = 0xabcd5678;
    oss << "Hex number: " << sugo::common::ios::hex(int32Number);
    EXPECT_EQ("Hex number: abcd5678", oss.str());
    oss.str("");
    uint16_t int16Number = 0xef12;
    oss << "Hex number: " << sugo::common::ios::hex(int16Number);
    EXPECT_EQ("Hex number: ef12", oss.str());
}

TEST_F(CommonTest, JsonNumeric)
{
    const unsigned value = 1234u;
    Json           json({{test::TestId, value}});
    const auto     dump = json.dump();
    EXPECT_STREQ(dump.c_str(), "{\"testid\":1234}");
}

TEST_F(CommonTest, JsonEnum)
{
    enum TestEnum : unsigned
    {
        Zero,
        One,
        Two,
        Three
    };
    const TestEnum value = Two;
    Json           json({{test::TestId, value}});
    const auto     dump = json.dump();
    EXPECT_STREQ(dump.c_str(), "{\"testid\":2}");
}