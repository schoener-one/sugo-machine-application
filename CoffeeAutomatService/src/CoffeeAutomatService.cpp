/*
 * CoffeeAutomatService.cpp
 *
 *  Created on: 01.09.2019
 *      Author: denis
 */

#include "Globals.hpp"
#include "CoffeeAutomatService.hpp"

#include <string>
#include <iostream>
#include <memory>
#include <cassert>

#include <jsonrpcpp/jsonrpcpp.hpp>

using namespace moco;


CoffeeAutomatService::CoffeeAutomatService(const std::string & serviceAddress,
		ICommandMessageBroker& messageBroker)
:CommunicationService(std::string("tcp://") + serviceAddress)
,m_messageBroker(messageBroker)
{
}

bool CoffeeAutomatService::processMessage(const std::string& requestMessage)
{
	std::string responseMessage;
	LOG(info) << "Received request: '" << requestMessage << "'";
	try
	{
		jsonrpcpp::entity_ptr entity = jsonrpcpp::Parser::do_parse(requestMessage);
		if (entity->is_request())
		{
			jsonrpcpp::request_ptr request = std::dynamic_pointer_cast<jsonrpcpp::Request>(entity);
			jsonrpcpp::Response response;
			if (m_messageBroker.hasReceiver(request->method()))
			{
				message::Command command;
				command.set_name(request->method());
				command.set_id(request->id().int_id());
				const jsonrpcpp::Parameter& parameter = request->params();
				if (!parameter.is_null())
				{
					command.set_parameters(parameter.to_json().dump());
				}
				const message::CommandResponse& commandResponse =
						m_messageBroker.send(command, request->method());
				if (commandResponse.result() == message::CommandResponse_Result_SUCCESS)
				{
					Json json;
					json["code"] = commandResponse.result();
					if (!commandResponse.response().empty())
					{
						json["data"] = Json::parse(commandResponse.response());
					}
					response = jsonrpcpp::Response(*request, json);
				}
				else
				{
					response = jsonrpcpp::Response(*request,
						jsonrpcpp::Error(Json{
							{"message", commandResponse.response()},
							{"code", commandResponse.result()}
						}));
				}
			}
			else
			{
				response = jsonrpcpp::Response(*request,
					jsonrpcpp::Error(Json{
						{"message", "unknown command"},
						{"code", message::CommandResponse_Result_INVALID_COMMAND}}));
			}
			responseMessage = response.to_json().dump();
		}
	}
	catch (jsonrpcpp::ParseErrorException& ex)
	{
		LOG(error) << ex.what();
	}
	LOG(info) << "Sending response: '" << responseMessage << "'";
	return sendMessage(responseMessage);
}
