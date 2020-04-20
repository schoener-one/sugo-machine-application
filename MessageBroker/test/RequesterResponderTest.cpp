///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   29.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <istream>
#include <ostream>
#include <thread>

#include "Logger.hpp"
#include "Requester.hpp"
#include "Responder.hpp"

namespace moco
{
class TestMessageHandler : public Responder::IMessageHandler
{
public:
    bool processReceived(StreamBuffer& inBuf, StreamBuffer& outBuf) override
    {
        std::istream in(&inBuf);
        std::getline(in, m_receivedMessage);
        std::ostream out(&outBuf);
        out << m_sendMessage;
        return true;
    }

    std::string m_receivedMessage;
    std::string m_sendMessage;
};

const static std::string address = "inproc://responder";
}  // namespace moco

using namespace moco;

class RequesterResponderTest : public ::testing::Test
{
protected:
    RequesterResponderTest() {}

    ~RequesterResponderTest() override {}

    static void SetUpTestCase() { Logger::init(Logger::Severity::error); }

    void SetUp() override {}

    void TearDown() override {}

    IOContext m_ioContext;
};

TEST_F(RequesterResponderTest, Responder_StartStop)
{
    TestMessageHandler handler;
    Responder          responder(address, handler, m_ioContext);
    EXPECT_TRUE(responder.start());
    EXPECT_TRUE(responder.isRunning());
    responder.stop();
    EXPECT_FALSE(responder.isRunning());
}

TEST_F(RequesterResponderTest, Responder_ReplyRequest)
{
    TestMessageHandler handler;
    Responder          responder(address, handler, m_ioContext);
    Requester          requester(m_ioContext);
    handler.m_sendMessage = "I feel fine!";
    EXPECT_TRUE(responder.start());
    std::thread t([&] {
        Logger::init(Logger::Severity::error);
        m_ioContext.run_one();
        m_ioContext.run_one();
        m_ioContext.run_one();
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(requester.connect(address));
    StreamBuffer outBuf;
    std::ostream os(&outBuf);
    os << "How are you?";
    StreamBuffer inBuf;
    std::istream is(&inBuf);
    EXPECT_TRUE(requester.send(outBuf, inBuf));
    std::string response;
    std::getline(is, response);
    EXPECT_TRUE(requester.disconnect(address));
    t.join();
    EXPECT_EQ(response, handler.m_sendMessage);
}
