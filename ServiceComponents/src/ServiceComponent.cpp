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

std::ostream& sugo::operator<<(std::ostream& ostr, const ServiceComponent::CommandId& commandId)
{
    ostr << commandId.getReceiverId() << "." << commandId.getId();
    return ostr;
}

std::ostream& sugo::operator<<(std::ostream&                           ostr,
                               const ServiceComponent::NotificationId& notificationId)
{
    ostr << notificationId.getId();
    return ostr;
}

message::CommandResponse ServiceComponent::createUnsupportedCommandResponse(
    const message::Command& command)
{
    LOG(warning) << "Received unhandled command '" << command.name() << "'";
    return message::createUnsupportedCommandResponse(command);
}

message::CommandResponse ServiceComponent::send(const ServiceComponent::CommandId& commandId,
                                                const Json&                        parameters)
{
    message::Command command;
    command.set_name(commandId.getId());
    if (!parameters.empty())
    {
        command.set_parameters(parameters.dump());
    }
    message::CommandResponse commandResponse;
    const bool success = m_messageBroker.send(command, commandId.getReceiverId(), commandResponse);
    assert(success);
    return commandResponse;
}

message::CommandResponse ServiceComponent::forward(const ServiceComponent::CommandId& commandId,
                                                   const message::Command&            command)
{
    message::Command forwardCommand = command;
    forwardCommand.set_name(commandId.getId());
    message::CommandResponse commandResponse;
    const bool               success =
        m_messageBroker.send(forwardCommand, commandId.getReceiverId(), commandResponse);
    assert(success);
    return commandResponse;
}

bool ServiceComponent::notify(const ServiceComponent::NotificationId& notificationId,
                              const Json&                             parameters)
{
    message::Command command;
    command.set_name(notificationId.getId());
    if (!parameters.empty())
    {
        command.set_parameters(parameters.dump());
    }
    return m_messageBroker.notify(command, notificationId.getReceiverIdList());
}
