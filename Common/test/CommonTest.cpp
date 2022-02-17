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

using namespace sugo;

class CommonTest : public ::testing::Test
{
protected:
    CommonTest() {}

    ~CommonTest() override {}

    static void SetUpTestCase() { Logger::init(); }

    void SetUp() override {}

    void TearDown() override {}
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
