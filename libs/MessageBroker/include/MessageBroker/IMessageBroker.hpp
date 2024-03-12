///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>

#include "Common/IRunnable.hpp"
#include "MessageBroker/Message.hpp"

namespace sugo::message_broker
{
/// @brief Interface class for sending and receiving messages.
class IMessageBroker : public common::IRunnable
{
protected:
    ~IMessageBroker() override = default;

public:
    /// @brief Handler definition which handles a request message.
    using RequestMessageHandler = std::function<ResponseMessage(const Message&)>;
    /// @brief Handler definition which handles a notification message.
    using NotificationMessageHandler = std::function<void(const Message&)>;
    /// @brief Runnable function type.
    using Runnable = std::function<void()>;

    /**
     * @brief Sends notification messages to every subscriber.
     *
     * @param message Notification message.
     * @param topicId Notification topic identifier.
     * @return true If the notification was successfully sent.
     * @return false If the notification was not successfully sent.
     */
    virtual bool notify(Message& message, const Topic& topicId) = 0;

    /**
     * @brief Subscribes to a publisher topic.
     * After this instance has subscribed to a publisher successfully notification messages
     * could be received by an registered MessageHandler.
     *
     * @param publisherAddress Publisher address to subscribe to.
     * @param topicId   Publisher topic to subscribe to.
     * @return true     If successfully subscribed.
     * @return false    If not successfully subscribed.
     * @see message_broker::Subscriber
     */
    virtual bool subscribe(const Address& publisherAddress, const Topic& topicId) = 0;

    /**
     * @brief Unsubscribes to a publisher topic.
     *
     * @param publisherAddress Publisher address to subscribe to.
     * @param topicId   Publisher topic to subscribe to.
     * @return true     If successfully unsubscribed.
     * @return false    If not successfully unsubscribed.
     */
    virtual bool unsubscribe(const Address& publisherAddress, const Topic& topicId) = 0;

    /**
     * @brief Sends a synchronous request message to a receiver instance and returns there response.
     *
     * @param message         Message to be sent.
     * @param receiverAddress Receiver id which should receive the message.
     * @param response [out]  Response message received from a request.
     * @return true if the request could be sent.
     */
    virtual bool send(Message& message, const Address& receiverAddress,
                      ResponseMessage& response) = 0;

    /**
     * @brief Registers a new request message handler.
     *
     * @param handler   Request message handler to be invoked on message receive.
     */
    virtual void registerRequestMessageHandler(RequestMessageHandler& handler) = 0;

    /**
     * @brief Registers a new request message handler.
     *
     * @param handler   Request message handler to be invoked on message receive.
     */
    virtual void registerRequestMessageHandler(RequestMessageHandler&& handler) = 0;

    /**
     * @brief Registers a new notification message handler.
     *
     * @param handler   Notification message handler to be invoked on message receive.
     */
    virtual void registerNotificationMessageHandler(NotificationMessageHandler& handler) = 0;

    /**
     * @brief Registers a new notification message handler.
     *
     * @param handler   Notification message handler to be invoked on message receive.
     */
    virtual void registerNotificationMessageHandler(NotificationMessageHandler&& handler) = 0;

    /**
     * @brief Unregisters an existing request message handler.
     */
    virtual void unregisterRequestMessageHandler() = 0;

    /**
     * @brief Unregisters an existing notification message handler.
     */
    virtual void unregisterNotificationMessageHandler() = 0;
};

}  // namespace sugo::message_broker
