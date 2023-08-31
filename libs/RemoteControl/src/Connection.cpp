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

#include <mongoose.h>
#include <cassert>
#include <iomanip>

#include "Common/Logger.hpp"
#include "RemoteControl/Connection.hpp"

namespace
{
std::string toString(const mg_str &str)
{
    return std::string(str.ptr, str.len);
}

const uint8_t WebsocketOpMask = 0x0f;
}  // namespace

using namespace sugo::remote_control;

IClientRequestHandler::ClientId Connection::getClientId() const
{
    return m_connection->id;
}

void Connection::handleHttpMessage(mg_http_message *message, std::string &docRoot)
{
    assert(m_connection != nullptr);
    if (mg_http_match_uri(message, "/websocket"))
    {
        LOG(debug) << getClientId() << ": Websocket update request received";

        // Upgrade to websocket. From now on, a m_connection is a full-duplex
        // Websocket m_connection, which will receive MG_EV_WS_MSG events.
        mg_ws_upgrade(m_connection, message, nullptr);  // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
    else if (mg_http_match_uri(message, "/state"))
    {
        if (mg_match(message->method, mg_str("GET"), nullptr))
        {
            LOG(debug) << getClientId() << "Event request received";
        }
    }
    else
    {
        // Handle normal HTTP requests
        struct mg_http_serve_opts opts
        {
            docRoot.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr
        };
        mg_http_serve_dir(m_connection, message, &opts);
    }
}

void Connection::handleError(const char *errorMessage)
{
    assert(m_connection != nullptr);
    assert(errorMessage != nullptr);
    const std::string label(m_connection->label);
    const std::string error(errorMessage);
    LOG(error) << getClientId() << ": " << error;
}

void Connection::handleWebsocketOpen(mg_http_message *)
{
    assert(m_connection != nullptr);
    m_isOpenWebsocket = true;
    LOG(debug) << getClientId() << ": Open websocket connection";
}

void Connection::handleWebsocketMessage(mg_ws_message *message, IClientRequestHandler &handler)
{
    assert(m_connection != nullptr);
    LOG(debug) << getClientId() << ": Websocket message received (" << std::hex << std::setw(2)
               << std::setfill('0') << static_cast<unsigned>(message->flags) << ")";

    switch (message->flags & WebsocketOpMask)
    {
        // WEBSOCKET_OP_CONTINUE 0
        // WEBSOCKET_OP_TEXT 1
        // WEBSOCKET_OP_BINARY 2
        // WEBSOCKET_OP_CLOSE 8
        // WEBSOCKET_OP_PING 9
        // WEBSOCKET_OP_PONG 10
        case WEBSOCKET_OP_TEXT:
            handleWebsocketMessage(message->data, handler);
            break;
        case WEBSOCKET_OP_PING:
            mg_ws_send(m_connection, "", 0, WEBSOCKET_OP_PONG);
            break;
        default:
            LOG(warning) << getClientId() << ": Unhandled websocket message";
            break;
    }
}

void Connection::handleWebsocketMessage(const mg_str &message, IClientRequestHandler &handler)
{
    assert(m_connection != nullptr);

    common::Json       response;
    const common::Json request = common::Json::parse(toString(message));

    if (handler.receiveRequest(getClientId(), request, response))
    {
        sendMessage(response);
    }
}

void Connection::sendMessage(const common::Json &response)
{
    assert(m_connection != nullptr);
    const std::string message = response.dump();
    LOG(debug) << getClientId() << ": Sending response: '" << message << "'";
    const size_t sentBytes =
        mg_ws_send(m_connection, message.c_str(), message.size(), WEBSOCKET_OP_TEXT);

    if (message.size() > sentBytes)
    {
        LOG(warning) << getClientId() << ": Message not completely transmitted (" << message.size()
                     << "/" << sentBytes << ")";
    }
}

void Connection::handleWebsocketControl(mg_ws_message *message)
{
    assert(m_connection != nullptr);
    LOG(debug) << getClientId() << ": Websocket control message received (" << std::hex
               << std::setw(2) << std::setfill('0') << static_cast<unsigned>(message->flags) << ")";

    switch (message->flags & WebsocketOpMask)
    {
        case WEBSOCKET_OP_CLOSE:
            m_isOpenWebsocket = false;
            LOG(debug) << getClientId() << ": received close request";
            break;
        default:
            LOG(warning) << getClientId() << ": Unhandled websocket control message";
            break;
    }
}
