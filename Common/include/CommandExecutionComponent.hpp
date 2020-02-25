/*
 * CommandExcecutionComponent.hpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#ifndef COMMANDEXECUTIONCOMPONENT_HPP_
#define COMMANDEXECUTIONCOMPONENT_HPP_

#include <sstream>

#include "Globals.hpp"
#include "StateMachine.hpp"
#include "ICommandMessageBroker.hpp"

namespace moco
{

template <class StateT, class EventT>
class CommandExecutionComponent
{
protected:
	explicit CommandExecutionComponent(ICommandMessageBroker& messageBroker)
		:m_messageBroker(messageBroker)  {}

	static message::CommandResponse handleCommand(const message::Command& command, const EventT& event,
			moco::StateMachine<StateT, EventT>& fsm)
	{
		LOG(debug) << "Received command '" << command.name() << "'";
		message::CommandResponse response;
		response.set_id(command.id());
		response.set_result(message::CommandResponse_Result_SUCCESS);
		fsm.push(event, [&](const EventT& event, const StateT& state){
			LOG(warning) << "Invalid transition for '" << event << "' in state '"
					<< state << "'";
			response.set_result(message::CommandResponse_Result_INVALID_STATE);
		});
		return response;
	}

	static message::CommandResponse handleCommandGetState(const message::Command& command,
			moco::StateMachine<StateT, EventT>& fsm)
	{
		LOG(debug) << "Received command '" << command.name() << "'";
		message::CommandResponse response;
		response.set_id(command.id());
		response.set_result(message::CommandResponse_Result_SUCCESS);
		std::ostringstream oss;
		oss << "{\"state\":\"" << fsm.getCurrentState() << "\"}";
		response.set_response(oss.str());
		return response;
	}

	static message::CommandResponse handleRequestCommand(const message::Command& command,
			const std::string& responseData)
	{
		LOG(debug) << "Received request command '" << command.name() << "'";
		message::CommandResponse response;
		response.set_id(command.id());
		response.set_result(message::CommandResponse_Result_SUCCESS);
		response.set_response(responseData);
		return response;
	}

	static message::CommandResponse handleInvalidState(const message::Command& command, const StateT& state)
	{
		LOG(debug) << "Received command '" << command.name() << "' in invalid state " << state;
		return handleError(command, "invalid state",
				message::CommandResponse_Result_INVALID_STATE);
	}

	static message::CommandResponse handleInvalidCommand(const message::Command& command)
	{
		LOG(debug) << "Received invalid command '" << command.name() << "'";
		return handleError(command, "invalid command",
				message::CommandResponse_Result_INVALID_COMMAND);
	}

	static message::CommandResponse handleInvalidParameter(const message::Command& command,
			const std::string& hint = "invalid parameter")
	{
		LOG(debug) << "Received command '" << command.name()
				<< "' with invalid parameters '" << command.parameters() << "'";
		return handleError(command, hint, message::CommandResponse_Result_INVALID_PARAMETER);
	}

	static message::CommandResponse handleError(const message::Command& command,
			const std::string& errorMessage = "procedure failed",
			message::CommandResponse_Result errorCode = message::CommandResponse_Result_ERROR)
	{
		message::CommandResponse response;
		response.set_id(command.id());
		response.set_result(errorCode);
		response.set_response(errorMessage);
		return response;
	}

	void registerReceiver(const std::string& id, ICommandMessageBroker::Receiver&& receiver)
	{
		m_messageBroker.registerReceiver(id, receiver);
	}

	message::CommandResponse send(const std::string& receiverId, const std::string& parameters = "")
	{
		message::Command command;
		command.set_name(receiverId);
		if (!parameters.empty())
		{
			command.set_parameters(parameters);
		}
		return m_messageBroker.send(command, receiverId);
	}

	void notify(const std::string& senderId, const std::string& parameters = "")
	{
		message::Command command;
		command.set_name(senderId);
		if (!parameters.empty())
		{
			command.set_parameters(parameters);
		}
		return m_messageBroker.notify(command, senderId);
	}

private:
	ICommandMessageBroker& m_messageBroker;
};

} // namespace moco

#endif /* COMMANDEXECUTIONCOMPONENT_HPP_ */
