/*
 * CoffeeAutomatService.cpp
 *
 *  Created on: 01.09.2019
 *      Author: denis
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include <jsonrpcpp/jsonrpcpp.hpp>

#include "CoffeeAutomatService.hpp"
#include "Globals.hpp"

using namespace moco;

void CoffeeAutomatService::setConfiguration(IConfiguration& configuration)
{
    configuration.add(Option("coffee-automat-service.address", std::string("*"),
                             "Network address of the service"));
    configuration.add(Option("coffee-automat-service.port", 1234u, "Network port of the service"));
}

CoffeeAutomatService::CoffeeAutomatService(ICommandMessageBroker& messageBroker,
                                           const IConfiguration&  configuration)
    : m_messageBroker(messageBroker),
      m_serviceAddress(
          std::string("tcp://") +
          configuration.getOption("coffee-automat-service.address").get<std::string>() +
          std::to_string(configuration.getOption("coffee-automat-service.port").get<unsigned>()))
{
}

bool CoffeeAutomatService::start() { return CommunicationService::start(m_serviceAddress); }

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
            jsonrpcpp::Response    response;
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
                    response = jsonrpcpp::Response(
                        *request, jsonrpcpp::Error(Json{{"message", commandResponse.response()},
                                                        {"code", commandResponse.result()}}));
                }
            }
            else
            {
                response = jsonrpcpp::Response(
                    *request, jsonrpcpp::Error(
                                  Json{{"message", "unknown command"},
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
