/*
 * CommandExcecutionComponent.hpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#ifndef SERVICECOMPONENT_HPP_
#define SERVICECOMPONENT_HPP_

#include "ServiceComponent.hpp"

#include <sstream>

#include "CommandId.hpp"
#include "Globals.hpp"
#include "ICommandMessageBroker.hpp"
#include "IServiceComponent.hpp"

namespace moco
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
class ServiceComponent : public IServiceComponent
{
public:
    // IRunnable {{
    bool start() override { return m_messageBroker.start(); }

    void stop() override { m_messageBroker.stop(); }

    bool isRunning() const override { return m_messageBroker.isRunning(); }
    // IRunnable }}

protected:
    explicit ServiceComponent(ICommandMessageBroker& messageBroker) : m_messageBroker(messageBroker)
    {
    }

    static message::CommandResponse handleRequestCommand(const message::Command& command,
                                                         const std::string&      responseData)
    {
        LOG(debug) << "Received request command '" << command.name() << "'";
        message::CommandResponse response;
        response.set_id(command.id());
        response.set_result(message::CommandResponse_Result_SUCCESS);
        response.set_response(responseData);
        return response;
    }

    static message::CommandResponse handleInvalidCommand(const message::Command& command)
    {
        LOG(debug) << "Received invalid command '" << command.name() << "'";
        return handleError(command, "invalid command",
                           message::CommandResponse_Result_INVALID_COMMAND);
    }

    static message::CommandResponse handleInvalidParameter(
        const message::Command& command, const std::string& hint = "invalid parameter")
    {
        LOG(debug) << "Received command '" << command.name() << "' with invalid parameters '"
                   << command.parameters() << "'";
        return handleError(command, hint, message::CommandResponse_Result_INVALID_PARAMETER);
    }

    static message::CommandResponse handleError(
        const message::Command& command, const std::string& errorMessage = "procedure failed",
        message::CommandResponse_Result errorCode = message::CommandResponse_Result_ERROR)
    {
        message::CommandResponse response;
        response.set_id(command.id());
        response.set_result(errorCode);
        response.set_response(errorMessage);
        return response;
    }

    void registerCommandHandler(const CommandId&                 commandId,
                                ICommandMessageBroker::Handler&& handler)
    {
        m_messageBroker.registerHandler(commandId.command, handler);
    }

    message::CommandResponse send(const CommandId& commandId) { return send(commandId, ""); }

    message::CommandResponse send(const CommandId& commandId, const std::string& parameters)
    {
        message::Command command;
        command.set_name(commandId.command);
        if (!parameters.empty())
        {
            command.set_parameters(parameters);
        }
        message::CommandResponse commandResponse;
        const bool success = m_messageBroker.send(command, commandId.receiver, commandResponse);
        assert(success);
        return std::move(commandResponse);
    }

    bool notify(const CommandId& commandId) { return notify(commandId, ""); }

    bool notify(const CommandId& commandId, const std::string& parameters)
    {
        message::Command command;
        command.set_name(commandId.command);
        if (!parameters.empty())
        {
            command.set_parameters(parameters);
        }
        return m_messageBroker.notify(command);
    }

    ICommandMessageBroker& getCommandMessageBroker() { return m_messageBroker; }

private:
    ICommandMessageBroker& m_messageBroker;
};

}  // namespace moco

#endif  // SERVICECOMPONENT_HPP_
