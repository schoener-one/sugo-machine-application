///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef ICOMMANDMESSAGEBROKERMOCK_HPP_
#define ICOMMANDMESSAGEBROKERMOCK_HPP_

#include <string>

#include <gmock/gmock.h>

#include "ICommandMessageBroker.hpp"

namespace moco
{
class ICommandMessageBrokerMock
    : public IMessageBroker<message::Command, message::CommandResponse, std::string>
{
public:
    using typename IMessageBroker<message::Command, message::CommandResponse, std::string>::Handler;

    MOCK_METHOD1(notify, bool(const message::Command&));
    MOCK_METHOD3(send,
                 bool(const message::Command&, const std::string&, message::CommandResponse&));
    MOCK_METHOD2(registerHandler, void(const std::string&, Handler&));
    MOCK_METHOD2(registerHandler, void(const std::string&, Handler&&));
    MOCK_METHOD1(unregisterHandler, void(const std::string& messageId));
    MOCK_CONST_METHOD1(hasRegisteredHandler, bool(const std::string&));
    MOCK_METHOD0(start, bool());
    MOCK_METHOD0(stop, void());
    MOCK_CONST_METHOD0(isRunning, bool());
};

}  // namespace moco

#endif  // ICOMMANDMESSAGEBROKERMOCK_HPP_
