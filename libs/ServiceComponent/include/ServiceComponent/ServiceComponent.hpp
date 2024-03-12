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

#include "Common/Types.hpp"
#include "MessageBroker/IMessageBroker.hpp"
#include "MessageBroker/Message.hpp"
#include "MessageBroker/MessageHelper.hpp"
#include "ServiceComponent/IProperty.hpp"
#include "ServiceComponent/IServiceComponent.hpp"

namespace sugo::service_component
{
/// @brief Class for receiving messages which are translated to appropriate actions.
class ServiceComponent : public IServiceComponent
{
public:
    /**
     * @brief Construct a new service component object
     *
     * @param messageBroker   The message broker to be used for sending and receiving.
     * @param subscriptionIds List of notification ids to subscribe to.
     * // TODO Remove notification id list from constructor. Use an abstract member function call
     * instead.
     */
    explicit ServiceComponent(message_broker::IMessageBroker& messageBroker,
                              const NotificationIdList&       subscriptionIds);
    ~ServiceComponent() override = default;

    bool start() override;

    void stop() override
    {
        m_messageBroker.stop();
    }

    bool isRunning() const override
    {
        return m_messageBroker.isRunning();
    }

    message_broker::IMessageBroker& getMessageBroker() override
    {
        return m_messageBroker;
    }

protected:
    /**
     * @brief Helper function to send request messages.
     *
     * @param requestId Request id object.
     * @param parameters Request parameters.
     * @param response [out] Request response message.
     * @return true If the request could be sent successfully.
     * @return false If the request could not be sent successfully.
     */
    bool send(const RequestId& requestId, const common::Json& parameters,
              message_broker::ResponseMessage& response);

    /**
     * @brief Helper function to send request messages.
     *
     * @param requestId Request id object.
     * @param response [out] Request response message.
     * @return true If the request could be sent successfully.
     * @return false If the request could not be sent successfully.
     */
    bool send(const RequestId& requestId, message_broker::ResponseMessage& response)
    {
        return send(requestId, common::Json(), response);
    }

    /**
     * @brief Helper function to send request messages.
     *
     * @param requestId Request id object.
     * @return true If the request could be sent successfully.
     * @return false If the request could not be sent successfully.
     */
    bool send(const RequestId& requestId)
    {
        message_broker::ResponseMessage responseMessage{};
        return send(requestId, common::Json(), responseMessage);
    }

    /**
     * @brief Helper function to send request messages.
     *
     * @param requestId Request id object.
     * @param parameters Request parameters.
     * @return true If the request could be sent successfully.
     * @return false If the request could not be sent successfully.
     */
    bool send(const RequestId& requestId, const common::Json& parameters)
    {
        message_broker::ResponseMessage responseMessage{};
        return send(requestId, parameters, responseMessage);
    }

    /**
     * @brief Helper function to forward a request to another component.
     *
     * @param requestId Request id object.
     * @param parameters Request parameters.
     * @return Response message object.
     */
    message_broker::ResponseMessage forward(const RequestId&               requestId,
                                            const message_broker::Message& message)
    {
        message_broker::Message forwardMessage{message};
        forwardMessage.setId(requestId.getMessageId());
        message_broker::ResponseMessage responseMessage{};
        m_messageBroker.send(forwardMessage, requestId.getAddress(), responseMessage);
        return responseMessage;
    }

    /**
     * @brief Helper function to send a notification message.
     *
     * @param notificationId Notification id object
     * @param parameters Notification parameters.
     * @return true If the notification could be sent successfully.
     * @return false If the notification could not be sent successfully.
     */
    bool notify(const NotificationId& notificationId,
                const common::Json&   parameters = common::Json());

    /**
     * @brief Method to handle property read request messages.
     *
     * @param message Received message to handle.
     * @param property Property to be read.
     * @return Request response message.
     */
    template<typename TypeT>
    message_broker::ResponseMessage handleGetPropertyRequestMessage(
        const message_broker::Message& message, const IProperty<TypeT>& property)
    {
        return message_broker::createResponseMessage(message, property.getValueAsJson());
    }

    /**
     * @brief Method to handle property read request messages.
     *
     * @param message Received message to handle.
     * @param property Property to be read.
     * @return Request response message.
     */
    template<typename TypeT>
    message_broker::ResponseMessage handleSetPropertyRequestMessage(
        const message_broker::Message& message, IProperty<TypeT>& property)
    {
        if (property.setValueFromJson(message.getPayload()))
        {
            LOG(debug) << "New property " << property.getName()
                       << " value set: " << property.getValueAsJson();
            return message_broker::createResponseMessage(message);
        }
        else
        {
            return message_broker::createErrorResponseMessage(
                message, message_broker::ResponseMessage::Result::InvalidPayload);
        }
    }

private:
    message_broker::IMessageBroker& m_messageBroker;  ///< Message broker object.
    const NotificationIdList& m_subscriptionIds;  ///< List of subscriptions to other components.
};
}  // namespace sugo::service_component
