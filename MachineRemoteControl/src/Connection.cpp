///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-09
 */
///////////////////////////////////////////////////////////////////////////////

#include "Connection.hpp"
#include "Logger.hpp"

#include <mongoose.h>
#include <cassert>
#include <iomanip>

namespace
{
std::string toString(const mg_str &str)
{
    return std::string(str.ptr, str.len);
}

const uint8_t WebsocketOpMask = 0x0f;
}  // namespace

using namespace sugo;
using namespace sugo::remote_control;

Connection::Identifier Connection::getId() const
{
    return m_connection->id;
}

void Connection::handleHttpMessage(mg_http_message *message, std::string &docRoot)
{
    assert(m_connection != nullptr);
    if (mg_http_match_uri(message, "/websocket"))
    {
        LOG(debug) << getId() << ": Websocket update request received";

        // Upgrade to websocket. From now on, a m_connection is a full-duplex
        // Websocket m_connection, which will receive MG_EV_WS_MSG events.
        mg_ws_upgrade(m_connection, message, nullptr);  // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
    else if (mg_http_match_uri(message, "/state"))
    {
        if (mg_match(message->method, mg_str("GET"), nullptr))
        {
            LOG(debug) << getId() << "Event request received";
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
    LOG(error) << getId() << ": " << error;
}

void Connection::handleWebsocketOpen(mg_http_message *)
{
    assert(m_connection != nullptr);
    m_isOpenWebsocket = true;
    LOG(debug) << getId() << ": Open websocket connection";
}

void Connection::handleWebsocketMessage(mg_ws_message *message, IRequestHandler &handler)
{
    assert(m_connection != nullptr);
    LOG(debug) << getId() << ": Websocket message received (" << std::hex << std::setw(2)
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
            LOG(warning) << getId() << ": Unhandled websocket message";
            break;
    }
}

void Connection::handleWebsocketMessage(const mg_str &message, IRequestHandler &handler)
{
    assert(m_connection != nullptr);

    Json       response;
    const Json request = Json::parse(toString(message));
    if (handler.receiveRequest(getId(), request, response))
    {
        sendMessage(response);
    }
}

void Connection::sendMessage(const Json &response)
{
    assert(m_connection != nullptr);
    const std::string message = response.dump();
    LOG(debug) << getId() << ": Sending response: '" << message << "'";
    const size_t sentBytes =
        mg_ws_send(m_connection, message.c_str(), message.size(), WEBSOCKET_OP_TEXT);
    if (message.size() > sentBytes)
    {
        LOG(warning) << getId() << ": Message not completely transmitted (" << message.size() << "/"
                     << sentBytes << ")";
    }
}

void Connection::handleWebsocketControl(mg_ws_message *message)
{
    assert(m_connection != nullptr);
    LOG(debug) << getId() << ": Websocket control message received (" << std::hex << std::setw(2)
               << std::setfill('0') << static_cast<unsigned>(message->flags) << ")";
    switch (message->flags & WebsocketOpMask)
    {
        case WEBSOCKET_OP_CLOSE:
            m_isOpenWebsocket = false;
            LOG(debug) << getId() << ": received close request";
            break;
        default:
            LOG(warning) << getId() << ": Unhandled websocket control message";
            break;
    }
}
