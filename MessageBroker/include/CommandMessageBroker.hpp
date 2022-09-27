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
#include "IOContext.hpp"
#include "MessageBroker.hpp"
#include "Server.hpp"

namespace sugo
{
/**
 * Class which handles command messages.
 * @todo Remove receiver-id list from interface.
 */
class CommandMessageBroker
    : public MessageBroker<message::Command, message::CommandResponse, std::string>,
      public Server::IMessageHandler
{
public:
    /**
     * @brief Construct a new command message broker object
     *
     * @param receiverId Receiver id to be used as an communication address.
     * @param ioContext  Io context to be used for receiving messages.
     */
    CommandMessageBroker(const std::string& receiverId, IOContext& ioContext);
    ~CommandMessageBroker() override;

    using typename MessageBroker<message::Command, message::CommandResponse, std::string>::Handler;

    bool send(const message::Command& message, const std::string& receiverId,
              message::CommandResponse& response) override;

    bool notify(const message::Command& message, const ReceiverIdList& receivers) override;

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
    inline static std::string createInProcessAddress(const std::string& receiverId)
    {
        return AddressPrefix + receiverId;
    }

protected:
    bool processReceived(StreamBuffer& in, StreamBuffer& out) override;
    void processPost() override;

private:
    bool send(const StreamBuffer& outBuf, message::CommandResponse& response,
              bool ignoreReceiveMessage = false);

    Server           m_server;       ///< Server instance
    Client           m_client;       ///< Client instance
    IOContext&       m_ioContext;    ///< Io context of the client and server instances.
    std::mutex       m_mutexClient;  ///< Mutex to restrict concurrent usage of the client.
    Thread::Runnable m_postProcess =
        nullptr;  ///< Callback function for message receive post processing.
    std::atomic_bool m_isProcessingReceiveMessage =
        false;  ///< Indicates if a received message is processed currently.
};

}  // namespace sugo
