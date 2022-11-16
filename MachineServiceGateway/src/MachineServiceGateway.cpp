///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   09.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include "MachineServiceGateway.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

#include <Command.pb.h>

#include "Globals.hpp"
#include "StringTokenizer.hpp"

namespace
{
constexpr unsigned MethodTokenCount      = 2u;  ///< method string: '<receiver-id>.<method-id>'
constexpr unsigned MethodTokenReceiverId = 0u;
constexpr unsigned MethodTokenMethodId   = 1u;
}  // namespace

using namespace sugo;
using namespace message;

void MachineServiceGateway::addConfigurationOptions(IConfiguration& configuration)
{
    static constexpr unsigned short defaultPortNumber = 1234u;
    configuration.add(Option("machine-service-gateway.address", std::string("*"),
                             "Network address of the service"));
    configuration.add(
        Option("machine-service-gateway.port", defaultPortNumber, "Network port of the service"));
}

MachineServiceGateway::MachineServiceGateway(ICommandMessageBroker& messageBroker,
                                             IOContext&             ioContext,
                                             const IConfiguration&  configuration)
    : ServiceComponent(messageBroker, {}),
      m_jsonRpcServer(
          std::string("tcp://") +
              configuration.getOption("machine-service-gateway.address").get<std::string>() + ":" +
              std::to_string(
                  configuration.getOption("machine-service-gateway.port").get<unsigned short>()),
          *this, ioContext)
{
}

bool MachineServiceGateway::start()
{
    bool success = ServiceComponent::start();  // starts also io context!

    if (success)
    {
        LOG(info) << "Starting JSON-RPC service on address: " << m_jsonRpcServer.getAddress();
        success = m_jsonRpcServer.start();
    }

    return success;
}

void MachineServiceGateway::stop()
{
    ServiceComponent::stop();
    m_jsonRpcServer.stop();
}

bool MachineServiceGateway::isRunning() const
{
    return ServiceComponent::isRunning() || m_jsonRpcServer.isRunning();
}

bool MachineServiceGateway::processReceived(StreamBuffer& inBuf, StreamBuffer& outBuf)
{
    std::string  requestMessage;
    std::istream in(&inBuf);
    std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(),
              std::back_inserter(requestMessage));
    LOG(info) << "Received request: '" << requestMessage << "'";
    bool success = false;

    try
    {
        jsonrpcpp::entity_ptr entity = jsonrpcpp::Parser::do_parse(requestMessage);
        if (entity->is_request())
        {
            jsonrpcpp::request_ptr request = std::dynamic_pointer_cast<jsonrpcpp::Request>(entity);
            const jsonrpcpp::Response response = processCommand(request);
            const std::string         responseMessage(response.to_json().dump());
            LOG(info) << "Sending response: '" << responseMessage << "'";
            std::ostream out(&outBuf);
            out << responseMessage;
            success = true;
        }
    }
    catch (jsonrpcpp::ParseErrorException& ex)
    {
        LOG(error) << ex.what();
    }
    return success;
}

jsonrpcpp::Response MachineServiceGateway::processCommand(const jsonrpcpp::request_ptr& request)
{
    StringTokenizer::Tokens tokens = StringTokenizer::split<'.'>(request->method());
    jsonrpcpp::Response     response;

    if (tokens.size() == MethodTokenCount && (tokens[MethodTokenReceiverId].size() > 0) &&
        (tokens[MethodTokenMethodId].size() > 0))
    {
        message::Command command;
        command.set_name(tokens[MethodTokenMethodId]);
        command.set_id(request->id().int_id());
        const jsonrpcpp::Parameter& parameters = request->params();
        if (!parameters.is_null())
        {
            command.set_parameters(parameters.to_json().dump());
        }
        message::CommandResponse commandResponse;
        const bool               success =
            getCommandMessageBroker().send(command, tokens[MethodTokenReceiverId], commandResponse);
        if (success && (commandResponse.result() == message::CommandResponse_Result_SUCCESS))
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
            *request,
            jsonrpcpp::Error(Json{{"message", "invalid format"},
                                  {"code", message::CommandResponse_Result_INVALID_COMMAND}}));
    }
    return response;
}
