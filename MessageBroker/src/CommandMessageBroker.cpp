///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <istream>
#include <ostream>

#include <Command.pb.h>

#include "CommandMessageBroker.hpp"

using namespace moco;

bool CommandMessageBroker::send(const message::Command& message, const std::string& receiverId,
                                message::CommandResponse& response)
{
    LOG(debug) << "Sending message to " << receiverId;
    const std::string address = createAddress(receiverId);
    bool              success = m_requester.connect(address);
    if (success)
    {
        StreamBuffer outBuf;
        std::ostream outStream(&outBuf);
        success = message.SerializeToOstream(&outStream);

        if (success)
        {
            StreamBuffer inBuf;
            success = m_requester.send(outBuf, inBuf);

            if (success)
            {
                std::istream inStream(&inBuf);
                success = response.ParseFromIstream(&inStream);

                if (!success)
                {
                    LOG(error) << "Failed to parse command response";
                }
            }
            else
            {
                LOG(error) << "Failed to send command";
            }
        }
        else
        {
            LOG(error) << "Failed to serialize command";
        }
        success = m_requester.disconnect(address) && success;
    }

    return success;
}

bool CommandMessageBroker::notify(const message::Command& message)
{
    message::CommandResponse response;
    bool                     success = true;
    for (auto receiverId : m_receiverIdList)
    {
        if (receiverId != m_receiverId)
        {
            // Ignore the response!
            success = send(message, receiverId, response) && success;
        }
    }
    return success;
}

bool CommandMessageBroker::processReceived(StreamBuffer& inBuf, StreamBuffer& outBuf)
{
    // TODO handle message IDs!
    message::Command command;
    std::istream     in(&inBuf);
    bool             success = command.ParseFromIstream(&in);
    if (success)
    {
        const std::string commandName = command.name();
        LOG(debug) << "Received command '" << commandName << "' (" << command.id() << ")";
        CommandMessageBrokerT::Handler* handler = findHandler(commandName);
        std::ostream                    out(&outBuf);
        if (handler != nullptr)
        {
            message::CommandResponse response = (*handler)(command);
            response.set_id(command.id());
            response.SerializeToOstream(&out);
        }
        else
        {
            LOG(error) << "Received invalid command: " << commandName;
            message::CommandResponse response;
            response.set_id(command.id());
            response.set_result(message::CommandResponse_Result_INVALID_COMMAND);
            response.set_response("unknown command");
            response.SerializeToOstream(&out);
            success = false;
        }
    }
    else
    {
        LOG(error) << "Failed to parse command";
    }
    return success;
}
