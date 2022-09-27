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
#include "Timer.hpp"

using namespace sugo;

CommandMessageBroker::CommandMessageBroker(const std::string& receiverId, IOContext& ioContext)
    : MessageBroker<message::Command, message::CommandResponse, std::string>(),
      m_server(createInProcessAddress(receiverId), *this, ioContext),
      m_client(ioContext),
      m_ioContext(ioContext)
{
}

CommandMessageBroker::~CommandMessageBroker()
{
    if (isRunning())
    {
        LOG(warning) << "Still running";
    }
}

bool CommandMessageBroker::start()
{
    assert(!m_ioContext.isRunning());
    if (!m_ioContext.start())
    {
        LOG(error) << "Failed to start io context";
        return false;
    }
    return m_server.start();
}

void CommandMessageBroker::stop()
{
    m_server.stop();
    m_ioContext.stop();
}

bool CommandMessageBroker::send(const message::Command& message, const std::string& receiverId,
                                message::CommandResponse& response)
{
    if (m_isProcessingReceiveMessage.load())
    {
        LOG(fatal) << "Failed to send message: processing received message (deadlock)";
        ASSERT_NOT_REACHABLE;
    }

    LOG(debug) << "Sending message " << message.name() << " to " << receiverId;
    const std::string                 address = createInProcessAddress(receiverId);
    const std::lock_guard<std::mutex> lock(m_mutexClient);

    if (!m_client.connect(address, MaxMessageTransmissionTime, MaxMessageTransmissionTime))
    {
        LOG(error) << "Failed to connect to " << address;
        return false;
    }

    StreamBuffer outBuf;
    std::ostream outStream(&outBuf);
    bool         success = message.SerializeToOstream(&outStream);

    if (!success)
    {
        LOG(error) << "Failed to serialize command";
    }

    if (success)
    {
        success = false;
        for (unsigned i = MaxMessageTransmissionRetries; !success && (i > 0); i--)
        {
            success =
                send(outBuf, response, (message.type() == message::Command_Type_Notification));
            if (!success)
            {
                LOG(error) << "Failed to send message (" << (MaxMessageTransmissionRetries - i + 1)
                           << "/" << MaxMessageTransmissionRetries << ")";
                std::this_thread::sleep_for(
                    MaxMessageTransmissionTime);  // just wait a short time to retry!
            }
        }
        if (!success)
        {
            LOG(error) << "Failed to send message completely";
        }
    }

    if (m_client.isConnected())
    {
        success = m_client.disconnect();
        if (!success)
        {
            LOG(error) << "Failed to disconnect";
        }
    }

    return success;
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
        // FIXME shouldn't return without reponse!
        LOG(error) << "Failed to parse command";
        return false;
    }
    m_isProcessingReceiveMessage = true;

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

    return response.SerializeToOstream(&out);
}

void CommandMessageBroker::processPost()
{
    m_isProcessingReceiveMessage = false;
    if (m_postProcess)
    {
        LOG(trace) << "Start post processing";
        m_postProcess();
    }
}