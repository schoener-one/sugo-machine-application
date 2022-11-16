///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <list>
#include <memory>
#include <mutex>

#include <Command.pb.h>

#include "Client.hpp"
#include "ICommandMessageBroker.hpp"
#include "IOContext.hpp"
#include "Publisher.hpp"
#include "Server.hpp"
#include "Subscriber.hpp"

namespace sugo::message
{
/**
 * Class which handles messages.
 *
 */
class CommandMessageBroker : public ICommandMessageBroker, public IMessageHandler
{
public:
    /// @brief Service types
    enum class Service
    {
        Responder,  ///< Responder which receives requests and sends the response
        Publisher,  ///< Publisher which publishes notifications
        Subscriber  ///< Subscriber which could subscribe to a notification publisher
    };

    /// @brief Maximum time to transmit a message (inclusive response).
    inline static constexpr std::chrono::milliseconds MaxMessageTransmissionTime{50};
    /// @brief Maximum of retries for a failed transmission.
    static constexpr unsigned MaxMessageTransmissionRetries = 3;

    /**
     * @brief Construct a new command message broker object
     *
     * @param address   Address of this broker instance.
     * @param ioContext Io context to be used for receiving messages.
     */
    CommandMessageBroker(const Address& address, IOContext& ioContext);
    ~CommandMessageBroker() override = default;

    bool send(const message::Command& message, const Address& receiverAddress,
              message::CommandResponse& response) override;
    bool notify(const message::Command& message, const MessageId& topic) override;
    bool subscribe(const Address& publisherAddress, const MessageId& topic) override;
    bool unsubscribe(const MessageId& topic) override;

    bool start() override;
    void stop() override;
    bool isRunning() const override
    {
        return m_server.isRunning() && m_publisher.isRunning() && m_ioContext.isRunning();
    }

    /**
     * @brief Registers a message post processing handler.
     * This handler is called after the message receive context is left,
     * which is generally after sending the response. Only in this
     * context the called instance is allowed to send messages again!
     *
     * @param postProcess Callback function to be called for post processing.
     */
    void registerPostProcessHandler(Thread::Runnable postProcess) override
    {
        m_postProcess = postProcess;
    }

    /**
     * @brief Creates a full qualified address based on the base address.
     * For inproc communication: inproc://<base-address>.<service-type[pub|sub|srv]>
     * For tcp communication:    tcp://<base-address>:<service-port>
     *
     * @param address Address without an prefix
     * @return        The full qualified address
     */
    static Address createFullQualifiedAddress(const Address& address, Service serviceType);

    /// @brief Handler type declaration
    using typename ICommandMessageBroker::MessageHandler;

    void registerMessageHandler(const MessageId& messageId, MessageHandler& handler) override;
    void registerMessageHandler(const MessageId& messageId, MessageHandler&& handler) override;
    void unregisterMessageHandler(const MessageId& messageId) override;
    bool hasRegisteredMessageHandler(const MessageId& messageId) const override;

protected:
    bool processReceived(StreamBuffer& in, StreamBuffer& out) override;
    void processPost() override;

private:
    MessageHandler* findHandler(const MessageId& messageId);
    bool            send(const StreamBuffer& outBuf, message::CommandResponse& response,
                         bool ignoreReceiveMessage = false);
    bool            processReceivedCommand(const message::Command& command, StreamBuffer& outBuf);
    bool            processReceivedNotification(const message::Command& command);

    using MessageReceiverMap = std::map<MessageId, MessageHandler>;

    Server             m_server;       ///< Server instance
    Client             m_client;       ///< Client instance
    Publisher          m_publisher;    ///< Publisher instance
    Subscriber         m_subscriber;   ///< Subscriber instance
    IOContext&         m_ioContext;    ///< Io context of the client and server instances.
    MessageReceiverMap m_handlers;     ///< Message handler map
    std::mutex         m_mutexClient;  ///< Mutex to restrict concurrent usage of the client.
    Thread::Runnable   m_postProcess =
        nullptr;  ///< Callback function for message receive post processing.
    std::atomic_bool m_isProcessingReceiveMessage =
        false;  ///< Indicates if a received message is processed currently.
};

}  // namespace sugo::message
