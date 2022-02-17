///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.01.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef IMESSAGEBROKER_HPP_
#define IMESSAGEBROKER_HPP_

#include <functional>

#include "IRunnable.hpp"

namespace sugo
{
/**
 * Interface class to handle messaging between components.
 * @tparam MessageT Message type.
 * @tparam ResponseT Message response type.
 * @tparam ReceiverIdT Receiver identifier type.
 */
template <class MessageT, class ResponseT = void, class ReceiverIdT = unsigned int>
class IMessageBroker : public IRunnable
{
protected:
    virtual ~IMessageBroker() {}

public:
    /// Handler definition which reacts to a request.
    using Handler = std::function<ResponseT(const MessageT&)>;

    /**
     * Notifies all receiver instances.
     * @param message Notification message.
     * @return true if the notification could be sent.
     */
    virtual bool notify(const MessageT& message) = 0;

    /**
     * Sends a synchronous request message to a receiver instance and returns there response.
     * @param message Send message.
     * @param receiverId Receiver id which should receive the message.
     * @param response Response of the request.
     * @return true if the request could be sent.
     */
    virtual bool send(const MessageT& message, const ReceiverIdT& receiverId,
                      ResponseT& response) = 0;

    /**
     * Registers a new message handler.
     * @param receiverId Message identifier.
     * @param handler Handler to be invoked on message receive.
     */
    virtual void registerHandler(const ReceiverIdT& messageId, Handler& handler) = 0;

    /**
     * Registers a new message handler.
     * @param receiverId Message identifier.
     * @param handler Handler to be invoked on message receive.
     */
    virtual void registerHandler(const ReceiverIdT& messageId, Handler&& handler) = 0;

    /**
     * Unregisters an existing message handler.
     * @param messageId Message identifier.
     */
    virtual void unregisterHandler(const ReceiverIdT& messageId) = 0;

    /**
     * Indicates if a handler has been registered for a message id.
     * @param messageId Message identifier.
     * @return true if the handler could be registered.
     */
    virtual bool hasRegisteredHandler(const ReceiverIdT& messageId) const = 0;
};

}  // namespace sugo

#endif /* IMESSAGEBROKER_HPP_ */
