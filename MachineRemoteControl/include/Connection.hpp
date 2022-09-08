///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-09
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Globals.hpp"
#include "IRequestHandler.hpp"

#include <string>

struct mg_connection;
struct mg_http_message;
struct mg_ws_message;
struct mg_str;

namespace sugo::remote_control
{
/**
 * @brief Class which represents a client connection.
 */
class Connection
{
public:
    /// @brief Uniq identifier for a connection
    using Identifier = IRequestHandler::ClientId;

    Connection()
    {
    }
    Connection(mg_connection *connection) : m_connection(connection)
    {
    }
    Connection(const Connection &) = default;
    Connection(Connection &&)      = default;

    Connection &operator=(const Connection &) = default;
    Connection &operator=(Connection &&) = default;

    Identifier getId() const;

    void sendMessage(const Json &response);
    void handleHttpMessage(mg_http_message *message, std::string &docRoot);
    void handleError(const char *errorMessage);
    void handleWebsocketOpen(mg_http_message *message);
    void handleWebsocketMessage(mg_ws_message *message, IRequestHandler &handler);
    void handleWebsocketMessage(const mg_str &message, IRequestHandler &handler);
    void handleWebsocketControl(mg_ws_message *message);

    bool isOpenWebsocket() const
    {
        return m_isOpenWebsocket;
    }

private:
    mg_connection *m_connection      = nullptr;
    bool           m_isOpenWebsocket = false;
};
}  // namespace sugo::remote_control