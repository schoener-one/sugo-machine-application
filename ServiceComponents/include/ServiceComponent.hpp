///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   17.11.2019
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CommandId.hpp"
#include "Globals.hpp"
#include "ICommandMessageBroker.hpp"
#include "IRunnable.hpp"
#include "ServiceComponent.hpp"
#include "MessageProtocol.hpp"
#include "MessageHelper.hpp"

#include <sstream>

namespace sugo
{
/**
 * Class to receive command messages and to translate them to events.
 *
 * @tparam StateT State type
 * @tparam EventT Event type
 *
 * @todo Needs to be transformed to a base class of all ServiceComponents in order
 *       to handle messages in a generic manner!
 */
class ServiceComponent : public IRunnable
{
public:
    // IRunnable {{
    bool start() override
    {
        return m_messageBroker.start();
    }

    void stop() override
    {
        m_messageBroker.stop();
    }

    bool isRunning() const override
    {
        return m_messageBroker.isRunning();
    }
    // IRunnable }}

protected:
    explicit ServiceComponent(ICommandMessageBroker&                       messageBroker,
                              const ICommandMessageBroker::ReceiverIdList& notifReceivers)
        : m_messageBroker(messageBroker), m_notificationReceivers(notifReceivers)
    {
    }

    static message::CommandResponse createUnsupportedCommandResponse(
        const message::Command& command)
    {
        LOG(debug) << "Received invalid command '" << command.name() << "'";
        return message::createUnsupportedCommandResponse(command);
    }

    static message::CommandResponse createResponse(
        const message::Command& command, const Json& response = Json(),
        const message::CommandResponse_Result result = message::CommandResponse_Result_SUCCESS)
    {
        return message::createResponse(command, response, result);
    }

    static message::CommandResponse createErrorResponse(
        const message::Command& command,
        const Json& errorMessage = Json({{protocol::IdErrorReason, protocol::IdErrorUnspecified}}),
        message::CommandResponse_Result errorCode = message::CommandResponse_Result_ERROR)
    {
        return message::createErrorResponse(command, errorMessage, errorCode);
    }

    void registerCommandHandler(const CommandId&                 commandId,
                                ICommandMessageBroker::Handler&& handler)
    {
        m_messageBroker.registerHandler(commandId.command, handler);
    }

    message::CommandResponse send(const CommandId& commandId)
    {
        return send(commandId, Json());
    }

    message::CommandResponse send(const CommandId& commandId, const Json& parameters)
    {
        message::Command command;
        command.set_name(commandId.command);
        if (!parameters.empty())
        {
            command.set_parameters(parameters.dump());
        }
        message::CommandResponse commandResponse;
        const bool success = m_messageBroker.send(command, commandId.receiver, commandResponse);
        assert(success);
        return commandResponse;
    }

    message::CommandResponse forward(const CommandId& commandId, const message::Command& command)
    {
        message::Command forwardCommand = command;
        forwardCommand.set_name(commandId.command);
        message::CommandResponse commandResponse;
        const bool               success =
            m_messageBroker.send(forwardCommand, commandId.receiver, commandResponse);
        assert(success);
        return commandResponse;
    }

    bool notify(const CommandId& commandId)
    {
        return notify(commandId, "");
    }

    bool notify(const CommandId& commandId, const Json& parameters)
    {
        message::Command command;
        command.set_name(commandId.receiver + "." + commandId.command);
        if (!parameters.empty())
        {
            command.set_parameters(parameters.dump());
        }
        return m_messageBroker.notify(command, m_notificationReceivers);
    }

    ICommandMessageBroker& getCommandMessageBroker()
    {
        return m_messageBroker;
    }

private:
    ICommandMessageBroker&                       m_messageBroker;
    const ICommandMessageBroker::ReceiverIdList& m_notificationReceivers;
};

}  // namespace sugo
