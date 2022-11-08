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

namespace sugo::message
{
/**
 * Generic interface class to send and receive messages.
 *
 * @tparam MessageT   Message type.
 * @tparam MessageIdT Message id type.
 * @tparam ResponseT  Message response type.
 * @tparam AddressT   Address type.
 */
template <typename MessageT, typename MessageIdT, typename ResponseT, typename AddressT>
class IGenericMessageBroker : public IRunnable
{
protected:
    virtual ~IGenericMessageBroker() = default;

public:
    /// @brief Handler definition which reacts to a request.
    using MessageHandler = std::function<ResponseT(const MessageT&)>;
    /// @brief Runnable function type.
    using Runnable = std::function<void()>;

    /**
     * @brief Sends notification messages to every subscriber.
     *
     * @param message Notification message.
     * @param topic The topic id for which the notification should be published.
     * @return true if the notification could be sent.
     */
    virtual bool notify(const MessageT& message, const MessageIdT& topic) = 0;

    /**
     * @brief Subscribes to a publisher topic.
     * After this instance has subscribed to a publisher successfully notification messages
     * could be received by an registered MessageHandler.
     *
     * @param publisher Publisher address to subscribe to.
     * @param topic   Publisher topic to subscribe to.
     * @return true     If successfully subscribed.
     * @return false    If not successfully subscribed.
     * @see message::Subscriber
     */
    virtual bool subscribe(const AddressT& publisher, const MessageIdT& topic) = 0;

    /**
     * @brief Unsubscribes to a publisher topic.
     *
     * @param topic   Publisher topic to unsubscribe from.
     * @return true     If successfully unsubscribed.
     * @return false    If not successfully unsubscribed.
     */
    virtual bool unsubscribe(const MessageIdT& topic) = 0;

    /**
     * @brief Sends a synchronous request message to a receiver instance and returns there response.
     *
     * @param message         Message to be sent.
     * @param receiverAddress Receiver id which should receive the message.
     * @param response        Response of the request.
     * @return true if the request could be sent.
     */
    virtual bool send(const MessageT& message, const AddressT& receiverAddress,
                      ResponseT& response) = 0;

    /**
     * @brief Registers a new message handler.
     *
     * @param messageId Message identifier.
     * @param handler   Handler to be invoked on message receive.
     */
    virtual void registerMessageHandler(const MessageIdT& messageId, MessageHandler& handler) = 0;

    /**
     * @brief Registers a new message handler.
     *
     * @param messageId Message identifier.
     * @param handler   Handler to be invoked on message receive.
     */
    virtual void registerMessageHandler(const MessageIdT& messageId, MessageHandler&& handler) = 0;

    /**
     * @brief Unregisters an existing message handler.
     *
     * @param messageId Message identifier.
     */
    virtual void unregisterMessageHandler(const MessageIdT& messageId) = 0;

    /**
     * @brief Indicates if a handler has been registered for a message id.
     *
     * @param messageId Message identifier.
     * @return true if the handler could be registered.
     */
    virtual bool hasRegisteredMessageHandler(const MessageIdT& messageId) const = 0;

    /**
     * @brief Registers a post process handler which will be called if a received message
     * has been handled.
     *
     * @param postProcess Callback function to be called for post processing.
     */
    virtual void registerPostProcessHandler(Runnable postProcess) = 0;
};

}  // namespace sugo::message
