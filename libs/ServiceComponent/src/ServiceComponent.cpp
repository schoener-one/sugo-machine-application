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

#include "ServiceComponent/ServiceComponent.hpp"
#include "Common/Logger.hpp"

using namespace sugo;
using namespace sugo::service_component;

ServiceComponent::ServiceComponent(message_broker::IMessageBroker& messageBroker,
                                   const NotificationIdList&       subscriptionIds)
    : m_messageBroker(messageBroker), m_subscriptionIds(subscriptionIds)
{
}

bool ServiceComponent::start()
{
    bool success = m_messageBroker.start();

    if (success and !m_subscriptionIds.empty())
    {
        for (const auto& notification : m_subscriptionIds)
        {
            if (!m_messageBroker.subscribe(notification.getAddress(), notification.getTopic()))
            {
                LOG(error) << "Failed to subscribe to " << notification;
                success = false;
            }
        }
    }

    return success;
}

bool ServiceComponent::send(const RequestId& requestId, const common::Json& parameters,
                            message_broker::ResponseMessage& response)
{
    message_broker::Message message;
    message.setId(requestId.getMessageId());

    if (!parameters.empty())
    {
        message.setPayload(parameters.dump());
    }

    LOG(debug) << "Sending request " << requestId;
    const bool success = m_messageBroker.send(message, requestId.getAddress(), response);

    return success && (response.getResult() == message_broker::ResponseMessage::Result::Success);
}

bool ServiceComponent::notify(const NotificationId& notificationId, const common::Json& parameters)
{
    message_broker::Message notification{};
    notification.setId(notificationId.getMessageId());

    if (!parameters.empty())
    {
        notification.setPayload(parameters.dump());
    }

    LOG(debug) << "Sending notification " << notificationId;
    return m_messageBroker.notify(notification, notificationId.getTopic());
}
