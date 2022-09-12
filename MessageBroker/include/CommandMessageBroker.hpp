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
    // cppcheck-suppress passedByValue
    CommandMessageBroker(const std::string& receiverId, AsioContext& ioContext)
        : MessageBroker<message::Command, message::CommandResponse, std::string>(),
          m_server(createInProcessAddress(receiverId), *this, ioContext),
          m_client(ioContext),
          m_ioContext(ioContext)
    {
    }

    ~CommandMessageBroker()
    {
    }

    using typename MessageBroker<message::Command, message::CommandResponse, std::string>::Handler;

    // IMessageBroker {{
    bool send(const message::Command& message, const std::string& receiverId,
              message::CommandResponse& response) override;

    bool notify(const message::Command& message, const ReceiverIdList& receivers) override;
    // IMessageBroker }}

    // IRunnable {{
    bool start() override
    {
        assert(!m_ioContext.isRunning());
        if (!m_ioContext.start())
        {
            LOG(error) << "Failed to start io context";
            return false;
        }
        return m_server.start();
    }

    void stop() override
    {
        m_server.stop();
        m_ioContext.stop();
    }

    bool isRunning() const override
    {
        return m_server.isRunning() && m_ioContext.isRunning();
    }
    // IRunnable }}

protected:
    // Server::IMessageHandler {{
    bool processReceived(StreamBuffer& in, StreamBuffer& out) override;
    // Server::IMessageHandler }}

private:
    inline static std::string createInProcessAddress(const std::string& receiverId)
    {
        std::string address("inproc://");
        address.append(receiverId);
        return address;
    }
    bool send(const StreamBuffer& outBuf, message::CommandResponse& response, bool ignoreReceiveMessage = false);

    Server       m_server;
    Client       m_client;
    AsioContext& m_ioContext;
    std::mutex   m_mutexClient;
};

}  // namespace sugo
