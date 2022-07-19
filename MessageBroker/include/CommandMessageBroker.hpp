///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   17.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDMESSAGEBROKER_HPP_
#define COMMANDMESSAGEBROKER_HPP_

#include <list>
#include <memory>

#include <Command.pb.h>

#include "Client.hpp"
#include "MessageBroker.hpp"
#include "Server.hpp"

namespace sugo
{
using CommandMessageBrokerT =
    MessageBroker<message::Command, message::CommandResponse, std::string>;

/**
 * Class which handles command messages.
 * @todo Remove receiver-id list from interface.
 */
class CommandMessageBroker : public CommandMessageBrokerT, public Server::IMessageHandler
{
public:   
    // cppcheck-suppress passedByValue
    CommandMessageBroker(const std::string& receiverId, IOContext& ioContext)
        : CommandMessageBrokerT(),
          m_server(createInProcessAddress(receiverId), *this, ioContext),
          m_client(ioContext),
          m_receiverId(receiverId)
    {
    }

    ~CommandMessageBroker()
    {
    }

    using typename CommandMessageBrokerT::Handler;

    // IMessageBroker {{
    bool send(const message::Command& message, const std::string& receiverId,
              message::CommandResponse& response) override;

    bool notify(const message::Command& message, const ReceiverIdList& receivers) override;
    // IMessageBroker }}

    // IRunnable {{
    bool start() override
    {
        return m_server.start();
    }

    void stop() override
    {
        m_server.stop();
    }

    bool isRunning() const override
    {
        return m_server.isRunning();
    }
    // IRunnable }}

    const std::string& getReceiverId() const
    {
        return m_receiverId;
    }

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

    Server            m_server;
    Client            m_client;
    const std::string m_receiverId;
};

}  // namespace sugo

#endif  // COMMANDMESSAGEBROKER_HPP_
