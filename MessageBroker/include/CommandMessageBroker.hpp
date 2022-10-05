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
#include "Server.hpp"

namespace sugo
{
/**
 * Class which handles command messages.
 * @todo Remove receiver-id list from interface.
 */
class CommandMessageBroker : public ICommandMessageBroker, public Server::IMessageHandler
{
public:
    /// @brief Address prefix
    inline static const std::string AddressPrefix{"inproc://"};
    /// @brief Maximum time to transmit a message (inclusive response).
    inline static constexpr std::chrono::milliseconds MaxMessageTransmissionTime{50};
    /// @brief Maximum of retries for a failed transmission.
    static constexpr unsigned MaxMessageTransmissionRetries = 3;

    /**
     * @brief Construct a new command message broker object
     *
     * @param receiverId Receiver id to be used as an communication address.
     * @param ioContext  Io context to be used for receiving messages.
     */
    CommandMessageBroker(const std::string& receiverId, IOContext& ioContext);
    ~CommandMessageBroker() override;

    bool send(const message::Command& message, const ReceiverId& receiverId,
              message::CommandResponse& response) override;
    bool notify(const message::Command& message, const ReceiverIdList& receiverIdList) override;

    bool start() override;
    void stop() override;
    bool isRunning() const override
    {
        return m_server.isRunning() && m_ioContext.isRunning();
    }

    /**
     * @brief Registers a message post processing handler.
     * This handler is called after the message receive context is left,
     * which is generally after sending the response. Only in this
     * context the called instance is allowed to send messages again!
     *
     * @param postProcess Callback function to be called for post processing.
     */
    void registerPostProcessHandler(Thread::Runnable postProcess)
    {
        m_postProcess = postProcess;
    }

    /**
     * @brief Creates a in-process address
     *
     * @param receiverId Receiver identifier
     * @return std::string Created in-process address.
     */
    inline static std::string createInProcessAddress(const ReceiverId& receiverId)
    {
        return AddressPrefix + receiverId;
    }

    /// @brief Handler type declaration
    using typename ICommandMessageBroker::Handler;

    void registerHandler(const MessageId& messageId, Handler& handler) override;
    void registerHandler(const MessageId& messageId, Handler&& handler) override;
    void unregisterHandler(const MessageId& messageId) override;
    bool hasRegisteredHandler(const MessageId& messageId) const;

protected:
    bool processReceived(StreamBuffer& in, StreamBuffer& out) override;
    void processPost() override;

private:
    Handler* findHandler(const ReceiverId& receiverId);
    bool     send(const StreamBuffer& outBuf, message::CommandResponse& response,
                  bool ignoreReceiveMessage = false);

    using MessageReceiverMap = std::map<MessageId, Handler>;

    Server             m_server;       ///< Server instance
    Client             m_client;       ///< Client instance
    IOContext&         m_ioContext;    ///< Io context of the client and server instances.
    MessageReceiverMap m_handlers;     ///< Message handler map
    std::mutex         m_mutexClient;  ///< Mutex to restrict concurrent usage of the client.
    Thread::Runnable   m_postProcess =
        nullptr;  ///< Callback function for message receive post processing.
    std::atomic_bool m_isProcessingReceiveMessage =
        false;  ///< Indicates if a received message is processed currently.

#ifndef NO_TEST_INTERFACE
    // TODO Use a derived CommandMessageBroker with additional test interface!

    ReceiverId m_notificationReceiver;  // Additional receiver id for testing

public:
    /**
     * @brief Sets an additional notification receiver id.
     * Can be used to listen to notification in a test environment
     *
     * @param receiverId Receiver id which receives all component notifications.
     */
    void setNotificationReceiver(const ReceiverId& receiverId)
    {
        m_notificationReceiver = receiverId;
    }
#endif  // NO_TEST_INTERFACE
};

}  // namespace sugo
