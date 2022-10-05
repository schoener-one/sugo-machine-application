///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.01.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <functional>
#include <vector>

#include "IRunnable.hpp"

namespace sugo
{
/**
 * Generic interface class to send and receive messages.
 * 
 * @tparam MessageT    Message type.
 * @tparam MessageIdT  Message id type.
 * @tparam ResponseT   Message response type.
 * @tparam ReceiverIdT Receiver identifier type.
 */
template <typename MessageT, typename MessageIdT, typename ResponseT = void,
          typename ReceiverIdT = unsigned int>
class IGenericMessageBroker : public IRunnable
{
protected:
    virtual ~IGenericMessageBroker() = default;

public:
    /// @brief Handler definition which reacts to a request.
    using Handler = std::function<ResponseT(const MessageT&)>;
    /// @brief Type definition for message id.
    using MessageId = MessageIdT;
    /// @brief Type definition for receiver id.
    using ReceiverId = ReceiverIdT;
    /// @brief Type definition for receiver list.
    using ReceiverIdList = std::vector<ReceiverIdT>;
    /// @brief Runnable function type.
    using Runnable = std::function<void()>;

    /**
     * Notifies all receiver instances.
     * @param message Notification message.
     * @param receivers List of receivers to be notified
     * @return true if the notification could be sent.
     */
    virtual bool notify(const MessageT& message, const ReceiverIdList& receivers) = 0;

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
     * @param messageId Message identifier.
     * @param handler Handler to be invoked on message receive.
     */
    virtual void registerHandler(const MessageIdT& messageId, Handler& handler) = 0;

    /**
     * Registers a new message handler.
     * @param messageId Message identifier.
     * @param handler Handler to be invoked on message receive.
     */
    virtual void registerHandler(const MessageIdT& messageId, Handler&& handler) = 0;

    /**
     * Unregisters an existing message handler.
     * @param messageId Message identifier.
     */
    virtual void unregisterHandler(const MessageIdT& messageId) = 0;

    /**
     * Indicates if a handler has been registered for a message id.
     * @param messageId Message identifier.
     * @return true if the handler could be registered.
     */
    virtual bool hasRegisteredHandler(const MessageIdT& messageId) const = 0;

    /**
     * @brief Registers a post process handler which will be called if a received message
     * has been handled.
     *
     * @param postProcess Callback function to be called for post processing.
     */
    virtual void registerPostProcessHandler(Runnable postProcess) = 0;
};

}  // namespace sugo
