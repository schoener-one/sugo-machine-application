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

#include "MessageBroker.hpp"
#include "Requester.hpp"
#include "Responder.hpp"

namespace moco
{
using CommandMessageBrokerT =
    MessageBroker<message::Command, message::CommandResponse, std::string>;

/**
 * Class which handles command messages.
 * @todo Remove receiver-id list from interface.
 */
class CommandMessageBroker : public CommandMessageBrokerT, public Responder::IMessageHandler
{
public:
    /// Receiver list definition.
    using ReceiverIdList = std::list<std::string>;

    static std::string createAddress(const std::string& receiverId)
    {
        std::string address("inproc://");
        address.append(receiverId);
        return std::move(address);
    }

    // cppcheck-suppress passedByValue
    CommandMessageBroker(const std::string& receiverId, const ReceiverIdList& receiverIdList,
                         IOContext& ioContext)
        : CommandMessageBrokerT(),
          m_responder(createAddress(receiverId), *this, ioContext),
          m_requester(ioContext),
          m_receiverId(receiverId),
          m_receiverIdList(receiverIdList)
    {
    }

    ~CommandMessageBroker() {}

    using typename CommandMessageBrokerT::Handler;

    // IMessageBroker {{
    bool send(const message::Command& message, const std::string& receiverId,
              message::CommandResponse& response) override;

    bool notify(const message::Command& message) override;
    // IMessageBroker }}

    // IRunnable {{
    bool start() override { return m_responder.start(); }

    void stop() override { m_responder.stop(); }

    bool isRunning() const override { return m_responder.isRunning(); }
    // IRunnable }}

    const std::string& getReceiverId() const { return m_receiverId; }

protected:
    // Responder {{
    bool processReceived(StreamBuffer& in, StreamBuffer& out) override;
    // Responder }}

private:
    Responder            m_responder;
    Requester            m_requester;
    const std::string    m_receiverId;
    const ReceiverIdList m_receiverIdList;
};

}  // namespace moco

#endif  // COMMANDMESSAGEBROKER_HPP_
