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

#pragma once

#include <jsonrpcpp/jsonrpcpp.hpp>
#include <string>

#include "Common/IOContext.hpp"
#include "Common/ServiceLocator.hpp"
#include "MessageBroker/Message.hpp"
#include "MessageBroker/Server.hpp"
#include "ServiceComponent/ServiceComponent.hpp"
#include "ServiceGateway/IServiceGateway.hpp"

namespace sugo::service_gateway
{
/// @brief Class to provide a service gateway for JSON RPC (v2.0) messages.
class ServiceGateway : public IServiceGateway, public service_component::ServiceComponent
{
public:
    /// @brief Identifier of the service gateway.
    static constexpr IServiceComponent::Identifier Identifier{"ServiceGateway"};

    /**
     * @brief Constructs a new service gateway object.
     *
     * @param messageBroker Message broker instance.
     * @param processContext Process context.
     * @param serviceLocator Service locator instance.
     * @param ioContext Process context to be use to run the JSON RPC server.
     *
     * @todo Pass only a ready configured IServer interface object.
     * @todo
     */
    ServiceGateway(message_broker::IMessageBroker& messageBroker,
                   common::IProcessContext&        processContext,
                   const common::ServiceLocator& serviceLocator, common::IOContext& ioContext);

    bool start() override;

    void stop() override;

    bool isRunning() const override;

private:
    bool receivedRequestMessage(message_broker::StreamBuffer& receivedMessage,
                                message_broker::StreamBuffer& responseMessage);

    jsonrpcpp::Response processRequestMessage(const jsonrpcpp::request_ptr& request);

    message_broker::Server m_jsonRpcServer;
};
}  // namespace sugo::service_gateway
