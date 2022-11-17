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

using namespace sugo::message;

namespace
{
/// @brief Address prefix
const std::string AddressPrefix{"inproc://"};

constexpr const char* convertToString(CommandMessageBroker::Service serviceType)
{
    switch (serviceType)
    {
        case CommandMessageBroker::Service::Responder:
            return ".srv";
        case CommandMessageBroker::Service::Publisher:
            return ".pub";
        case CommandMessageBroker::Service::Subscriber:
            return ".sub";
    }
    return "";
}
}  // namespace

Address CommandMessageBroker::createFullQualifiedAddress(const Address& address,
                                                         Service        serviceType)
{
    std::string fullQualifiedAddress = AddressPrefix + address + convertToString(serviceType);
    return fullQualifiedAddress;
}

CommandMessageBroker::CommandMessageBroker(const Address& address, IOContext& ioContext)
    : m_server(createFullQualifiedAddress(address, Service::Responder), *this, ioContext),
      m_client(ioContext),
      m_publisher(createFullQualifiedAddress(address, Service::Publisher), ioContext),
      m_subscriber(*this, ioContext),
      m_ioContext(ioContext)
{
}

bool CommandMessageBroker::start()
{
    assert(!m_ioContext.isRunning());
    if (!m_ioContext.start())
    {
        LOG(error) << "Failed to start io context";
        return false;
    }
    const bool success = m_publisher.start();
    return m_server.start() && success;
}

void CommandMessageBroker::stop()
{
    m_server.stop();
    m_publisher.stop();
    m_ioContext.stop();
}

bool CommandMessageBroker::send(const message::Command& message, const Address& address,
                                message::CommandResponse& response)
{
    if (m_isProcessingReceiveMessage.load())
    {
        LOG(fatal) << "Failed to send message: processing received message (deadlock)";
        return false;
    }

    const std::string fullAddress = createFullQualifiedAddress(address, Service::Responder);
    LOG(debug) << "Sending message " << message.name() << " to " << fullAddress;
    const std::lock_guard<std::mutex> lock(m_mutexClient);

    if (!m_client.connect(fullAddress, MaxMessageTransmissionTime, MaxMessageTransmissionTime))
    {
        LOG(error) << "Failed to connect to " << address;
        return false;
    }

    StreamBuffer outBuf;
    std::ostream outStream(&outBuf);
    bool         success = message.SerializeToOstream(&outStream);

    if (!success)
    {
        LOG(error) << "Failed to serialize message";
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
                const unsigned tryCount = MaxMessageTransmissionRetries - i + 1;
                LOG(error) << "Failed to send message " << message.name() << " to " << address
                           << " (" << tryCount << "/" << MaxMessageTransmissionRetries << ")";
                std::this_thread::sleep_for(MaxMessageTransmissionTime *
                                            tryCount);  // just wait a short time to retry!
            }
        }
        if (!success)
        {
            LOG(error) << "Failed to send message " << message.name() << " to " << address
                       << " completely";
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
        LOG(error) << "Failed to send message";
        return false;
    }

    if (!ignoreReceiveMessage)
    {
        std::istream inStream(&inBuf);
        if (!response.ParseFromIstream(&inStream))
        {
            LOG(error) << "Failed to parse message response";
            return false;
        }
    }

    return true;
}

bool CommandMessageBroker::notify(const message::Command& message, const MessageId& topic)
{
    message::Command notification(message);
    notification.set_type(message::Command_Type_Notification);
    LOG(debug) << "Notify message " << notification.name() << " from " << m_publisher.getAddress();

    StreamBuffer outBuf;
    std::ostream outStream(&outBuf);

    if (!message.SerializeToOstream(&outStream))
    {
        LOG(error) << "Failed to serialize message";
        return false;
    }

    return m_publisher.publish(topic, outBuf);
}

bool CommandMessageBroker::subscribe(const Address& address, const MessageId& topic)
{
    const auto fullAddress = createFullQualifiedAddress(address, Service::Publisher);
    LOG(debug) << "Subscribing to " << fullAddress << "/" << topic;
    return m_subscriber.subscribe(fullAddress, topic);
}

bool CommandMessageBroker::unsubscribe(const MessageId& topic)
{
    LOG(debug) << "Unsubscribing from " << topic;
    return m_subscriber.unsubscribe(topic);
}

bool CommandMessageBroker::processReceived(StreamBuffer& inBuf, StreamBuffer& outBuf)
{
    // TODO handle message IDs!
    message::Command message;
    std::istream     in(&inBuf);

    if (!message.ParseFromIstream(&in))
    {
        // FIXME shouldn't return without reponse!
        LOG(error) << "Failed to parse message";
        return false;
    }

    m_isProcessingReceiveMessage = true;

    if (message.type() == message::Command_Type_Notification)
    {
        return processReceivedNotification(message);
    }
    else
    {
        return processReceivedCommand(message, outBuf);
    }
}

bool CommandMessageBroker::processReceivedCommand(const message::Command& message,
                                                  StreamBuffer&           outBuf)
{
    std::ostream             out(&outBuf);
    message::CommandResponse response;
    MessageHandler*          handler = findHandler(message.name());

    if (handler != nullptr)
    {
        response = (*handler)(message);
    }
    else
    {
        LOG(error) << "Received unhandled message: " << message.name();
        response = message::createUnsupportedCommandResponse(message);
    }

    response.set_id(message.id());
    return response.SerializeToOstream(&out);
}

bool CommandMessageBroker::processReceivedNotification(const message::Command& message)
{
    MessageHandler* handler = findHandler(message.name());

    if (handler == nullptr)
    {
        LOG(error) << "Received unhandled notification: " << message.name();
        return false;
    }

    (void)(*handler)(message);
    return true;
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

void CommandMessageBroker::registerMessageHandler(const MessageId& messageId,
                                                  MessageHandler&  handler)
{
    m_handlers[messageId] = handler;
}

void CommandMessageBroker::registerMessageHandler(const MessageId& messageId,
                                                  MessageHandler&& handler)
{
    m_handlers[messageId] = handler;
}

void CommandMessageBroker::unregisterMessageHandler(const MessageId& messageId)
{
    auto iter = m_handlers.find(messageId);
    if (iter != m_handlers.end())
    {
        m_handlers.erase(iter);
    }
}

bool CommandMessageBroker::hasRegisteredMessageHandler(const MessageId& messageId) const
{
    auto iter = m_handlers.find(messageId);
    return (iter != m_handlers.end());
}

CommandMessageBroker::MessageHandler* CommandMessageBroker::findHandler(const MessageId& messageId)
{
    MessageHandler* handler = nullptr;
    auto            iter    = m_handlers.find(messageId);
    if (iter != m_handlers.end())
    {
        handler = &(iter->second);
    }
    return handler;
}