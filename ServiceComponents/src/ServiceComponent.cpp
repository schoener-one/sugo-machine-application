///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   17.11.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include "ServiceComponent.hpp"
#include "Logger.hpp"

using namespace sugo;

ServiceComponent::ServiceComponent(message::ICommandMessageBroker& messageBroker,
                                   const NotificationIdList&       subscriptionIds)
    : m_messageBroker(messageBroker), m_subscriptionIds(subscriptionIds)
{
}

bool ServiceComponent::start()
{
    bool success = m_messageBroker.start();

    if (success)
    {
        for (const auto& notification : m_subscriptionIds)
        {
            if (!m_messageBroker.subscribe(notification.getPublisherAddress(),
                                           notification.getId()))
            {
                LOG(error) << "Failed to subscribe to " << notification;
                success = false;
            }
        }
    }

    return success;
}

message::CommandResponse ServiceComponent::send(const message::CommandId& commandId,
                                                const Json&               parameters)
{
    message::Command command;
    command.set_name(commandId.getId());
    if (!parameters.empty())
    {
        command.set_parameters(parameters.dump());
    }
    message::CommandResponse commandResponse;
    const bool               success =
        m_messageBroker.send(command, commandId.getReceiverAddress(), commandResponse);
    assert(success);
    return commandResponse;
}

message::CommandResponse ServiceComponent::forward(const message::CommandId& commandId,
                                                   const message::Command&   command)
{
    message::Command forwardCommand = command;
    forwardCommand.set_name(commandId.getId());
    message::CommandResponse commandResponse;
    const bool               success =
        m_messageBroker.send(forwardCommand, commandId.getReceiverAddress(), commandResponse);
    assert(success);
    return commandResponse;
}

bool ServiceComponent::notify(const message::NotificationId& notificationId, const Json& parameters)
{
    message::Command command;
    command.set_name(notificationId.getId());
    if (!parameters.empty())
    {
        command.set_parameters(parameters.dump());
    }
    return m_messageBroker.notify(command, notificationId.getId());
}
