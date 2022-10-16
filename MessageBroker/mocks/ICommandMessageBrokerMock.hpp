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

namespace sugo::message
{
class ICommandMessageBrokerMock : public ICommandMessageBroker
{
public:
    using typename IGenericMessageBroker<message::Command, MessageId, message::CommandResponse,
                                         Address>::MessageHandler;

    MOCK_METHOD(bool, start, ());
    MOCK_METHOD(void, stop, ());
    MOCK_METHOD(bool, isRunning, (), (const));
    MOCK_METHOD(bool, notify, (const message::Command&, const MessageId&));
    MOCK_METHOD(bool, subscribe, (const Address&, const MessageId&));
    MOCK_METHOD(bool, unsubscribe, (const MessageId&));
    MOCK_METHOD(bool, send, (const message::Command&, const Address&, message::CommandResponse&));
    MOCK_METHOD(void, registerMessageHandler, (const MessageId&, MessageHandler&));
    MOCK_METHOD(void, registerMessageHandler, (const MessageId&, MessageHandler&&));
    MOCK_METHOD(void, unregisterMessageHandler, (const MessageId&));
    MOCK_METHOD(bool, hasRegisteredMessageHandler, (const MessageId&), (const));
    MOCK_METHOD(void, registerPostProcessHandler, (Runnable));
};

}  // namespace sugo::message
