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

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

#include "Common/StringTokenizer.hpp"
#include "Common/Types.hpp"
#include "ServiceGateway/Configuration.hpp"
#include "ServiceGateway/ServiceGateway.hpp"

using namespace sugo;
using namespace service_gateway;

namespace
{
constexpr unsigned MethodTokenCount           = 2u;  ///< method string: '<receiver-id>.<method-id>'
constexpr unsigned MethodTokenReceiverAddress = 0u;
constexpr unsigned MethodTokenMethodId        = 1u;

const service_component::IServiceComponent::NotificationIdList EmptyNotificationIdList{};
}  // namespace

ServiceGateway::ServiceGateway(
    message_broker::IMessageBroker& messageBroker,
    common::IProcessContext&,  // FIXME not used, but has to be passed by ExecutionBundle
    const common::ServiceLocator& serviceLocator, common::IOContext& ioContext)
    : ServiceComponent(messageBroker, EmptyNotificationIdList),
      m_jsonRpcServer(
          std::string("tcp://") +
              serviceLocator.get<common::IConfiguration>()
                  .getOption(id::ConfigNetworkAddress)
                  .get<std::string>() +
              ":" +
              std::to_string(serviceLocator.get<common::IConfiguration>()
                                 .getOption(id::ConfigNetworkPort)
                                 .get<unsigned short>()),
          [this](message_broker::StreamBuffer& receivedMessage,
                 message_broker::StreamBuffer& responseMessage) {
              return receivedRequestMessage(receivedMessage, responseMessage);
          },
          ioContext)
{
}

bool ServiceGateway::start()
{
    bool success = ServiceComponent::start();  // starts also io context!

    if (success)
    {
        LOG(info) << "Starting JSON-RPC service on address: " << m_jsonRpcServer.getAddress();
        success = m_jsonRpcServer.start();
    }

    return success;
}

void ServiceGateway::stop()
{
    ServiceComponent::stop();
    m_jsonRpcServer.stop();
}

bool ServiceGateway::isRunning() const
{
    return ServiceComponent::isRunning() || m_jsonRpcServer.isRunning();
}

bool ServiceGateway::receivedRequestMessage(message_broker::StreamBuffer& receivedMessageBuffer,
                                            message_broker::StreamBuffer& responseMessageBuffer)
{
    std::string  requestMessage;
    std::istream in(&receivedMessageBuffer);
    std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(),
              std::back_inserter(requestMessage));
    LOG(info) << "Received message: '" << requestMessage << "'";
    bool success = false;

    try
    {
        jsonrpcpp::entity_ptr entity = jsonrpcpp::Parser::do_parse(requestMessage);
        if (entity->is_request())
        {
            jsonrpcpp::request_ptr request = std::dynamic_pointer_cast<jsonrpcpp::Request>(entity);
            const jsonrpcpp::Response response = processRequestMessage(request);
            const std::string         responseMessage(response.to_json().dump());
            LOG(info) << "Sending response: '" << responseMessage << "'";
            std::ostream out(&responseMessageBuffer);
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

jsonrpcpp::Response ServiceGateway::processRequestMessage(const jsonrpcpp::request_ptr& request)
{
    common::StringTokenizer::Tokens tokens = common::StringTokenizer::split<'.'>(request->method());
    jsonrpcpp::Response             response;

    if (tokens.size() == MethodTokenCount && (tokens[MethodTokenReceiverAddress].size() > 0) &&
        (tokens[MethodTokenMethodId].size() > 0))
    {
        message_broker::Message                   message;
        const message_broker::Message::Identifier id =
            std::atoi(tokens[MethodTokenMethodId].c_str());
        message.setId(id);
        message.setSequence(request->id().int_id());
        const jsonrpcpp::Parameter& parameters = request->params();

        if (!parameters.is_null())
        {
            message.setPayload(parameters.to_json().dump());
        }

        message_broker::ResponseMessage responseMessage;
        const bool                      success =
            getMessageBroker().send(message, tokens[MethodTokenReceiverAddress], responseMessage);

        if (success &&
            (responseMessage.getResult() == message_broker::ResponseMessage::Result::Success))
        {
            common::Json json;
            json["code"] = responseMessage.getResult();

            if (!responseMessage.getPayload().empty())
            {
                json["data"] = common::Json::parse(responseMessage.getPayload());
            }

            response = jsonrpcpp::Response(*request, json);
        }
        else
        {
            response = jsonrpcpp::Response(
                *request, jsonrpcpp::Error(common::Json{{"message", responseMessage.getError()},
                                                        {"code", responseMessage.getResult()}}));
        }
    }
    else
    {
        response = jsonrpcpp::Response(
            *request, jsonrpcpp::Error(common::Json{
                          {"message", "invalid format"},
                          {"code", message_broker::ResponseMessage::Result::InvalidMessage}}));
    }
    return response;
}
