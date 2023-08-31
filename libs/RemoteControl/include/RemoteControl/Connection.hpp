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

#include <string>

#include "Common/Types.hpp"
#include "RemoteControl/IClientRequestHandler.hpp"

struct mg_connection;
struct mg_http_message;
struct mg_ws_message;
struct mg_str;

namespace sugo::remote_control
{
/// @brief Class which represents a client connection.
class Connection
{
public:
    /// @brief Default constructor
    Connection() = default;

    /**
     * @brief Construct a new Connection object.
     *
     * @param connection The mongoose connection to be used.
     */
    explicit Connection(mg_connection *connection) : m_connection(connection)
    {
    }

    /// @brief Default copy constructor.
    Connection(const Connection &) = default;

    /// @brief Default move constructor.
    Connection(Connection &&) = default;

    /// @brief Default assignment operator.
    Connection &operator=(const Connection &) = default;

    /// @brief Default move operator.
    Connection &operator=(Connection &&) = default;

    /**
     * @brief Returns the client identifier.
     *
     * @return The client identifier.
     */
    IClientRequestHandler::ClientId getClientId() const;

    /**
     * @brief Sends a response message to the client.
     *
     * @param response Response message JSON formatted.
     */
    void sendMessage(const common::Json &response);

    /**
     * @brief Handles a HTTP request message.
     *
     * @param message Message to handle.
     * @param docRoot Document root of the message.
     */
    void handleHttpMessage(mg_http_message *message, std::string &docRoot);

    /**
     * @brief Handles a error message.
     *
     * @param errorMessage Error message to handle.
     */
    void handleError(const char *errorMessage);

    /**
     * @brief Handles a websocket open message.
     *
     * @param message Websocket open message to handle.
     */
    void handleWebsocketOpen(mg_http_message *message);

    /**
     * @brief Handles a websocket message.
     *
     * @param message Websocket message to handle.
     * @param handler Client request handle to be informed.
     */
    void handleWebsocketMessage(mg_ws_message *message, IClientRequestHandler &handler);

    /**
     * @brief Handles a websocket message.
     *
     * @param message Websocket message to handle.
     * @param handler Client request handle to be informed.
     */
    void handleWebsocketMessage(const mg_str &message, IClientRequestHandler &handler);

    /**
     * @brief Handles a websocket control message.
     *
     * @param message Websocket control message to be handled.
     */
    void handleWebsocketControl(mg_ws_message *message);

    /**
     * @brief Indicates if the websocket connection is open.
     *
     * @return true The connection is established.
     * @return false The connection is not established.
     */
    bool isOpenWebsocket() const
    {
        return m_isOpenWebsocket;
    }

private:
    mg_connection *m_connection      = nullptr;  ///< Mongoose connection object.
    bool           m_isOpenWebsocket = false;    ///< Indicates if websocket is open.
};
}  // namespace sugo::remote_control