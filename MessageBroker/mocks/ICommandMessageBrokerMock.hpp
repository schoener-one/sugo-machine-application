///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <gmock/gmock.h>

#include "ICommandMessageBroker.hpp"

namespace sugo
{
class ICommandMessageBrokerMock : public ICommandMessageBroker
{
public:
    using typename IMessageBroker<message::Command, message::CommandResponse, std::string>::Handler;

    MOCK_METHOD(bool, notify, (const message::Command&, const std::vector<std::string>&));
    MOCK_METHOD(bool, send, (const message::Command&, const std::string&, message::CommandResponse&));
    MOCK_METHOD(void, registerHandler, (const std::string&, Handler&));
    MOCK_METHOD(void, registerHandler, (const std::string&, Handler&&));
    MOCK_METHOD(void, unregisterHandler, (const std::string&));
    MOCK_METHOD(bool, hasRegisteredHandler, (const std::string&), (const));
    MOCK_METHOD(bool, start, ());
    MOCK_METHOD(void, stop, ());
    MOCK_METHOD(bool, isRunning, (), (const));
    MOCK_METHOD(void, registerPostProcessHandler, (Thread::Runnable));
};

}  // namespace sugo
