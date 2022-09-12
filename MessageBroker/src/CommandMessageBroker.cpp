///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <istream>
#include <ostream>

#include <Command.pb.h>

#include "CommandMessageBroker.hpp"
#include "MessageHelper.hpp"
#include "MessageProtocol.hpp"

using namespace sugo;

bool CommandMessageBroker::send(const message::Command& message, const std::string& receiverId,
                                message::CommandResponse& response)
{
    LOG(debug) << "Sending message to " << receiverId;
    const std::string                 address = createInProcessAddress(receiverId);
    const std::lock_guard<std::mutex> lock(m_mutexClient);

    if (!m_client.connect(address))
    {
        LOG(error) << "Failed to connect to " << address;
        return false;
    }

    StreamBuffer outBuf;
    std::ostream outStream(&outBuf);
    bool         success = message.SerializeToOstream(&outStream);

    if (success)
    {
        success = send(outBuf, response, (message.type() == message::Command_Type_Notification));
    }
    else
    {
        LOG(error) << "Failed to serialize command";
    }

    return m_client.disconnect(address) && success;
}

bool CommandMessageBroker::send(const StreamBuffer& outBuf, message::CommandResponse& response,
                                bool ignoreReceiveMessage)
{
    StreamBuffer inBuf;
    if (!m_client.send(outBuf, inBuf))
    {
        LOG(error) << "Failed to send command";
        return false;
    }

    if (!ignoreReceiveMessage)
    {
        std::istream inStream(&inBuf);
        if (!response.ParseFromIstream(&inStream))
        {
            LOG(error) << "Failed to parse command response";
            return false;
        }
    }

    return true;
}

bool CommandMessageBroker::notify(const message::Command& message, const ReceiverIdList& receivers)
{
    message::CommandResponse response;
    bool                     success = true;
    message::Command         notification(message);
    notification.set_type(message::Command_Type_Notification);

    for (const auto& receiverId : receivers)
    {
        // Ignore the response!
        success = send(notification, receiverId, response) && success;
    }

    return success;
}

bool CommandMessageBroker::processReceived(StreamBuffer& inBuf, StreamBuffer& outBuf)
{
    // TODO handle message IDs!
    message::Command command;
    std::istream     in(&inBuf);

    if (!command.ParseFromIstream(&in))
    {
        LOG(error) << "Failed to parse command";
        return false;
    }

    const std::string& commandName = command.name();
    // LOG(debug) << "Received command '" << commandName << "' (" << command.id() << ")";
    Handler*                 handler = findHandler(commandName);
    std::ostream             out(&outBuf);
    message::CommandResponse response;

    if (handler == nullptr)
    {
        if (command.type() == message::Command_Type_Notification)
        {
            // TODO notification can be ignored if no handler is available!
            response = message::createResponse(command);  // but must be answered!
        }
        else
        {
            LOG(error) << "Received invalid command: " << commandName;
            response = message::createUnsupportedCommandResponse(command);
        }
    }
    else
    {
        response = (*handler)(command);
        response.set_id(command.id());
    }

    response.SerializeToOstream(&out);
    return true;
}
