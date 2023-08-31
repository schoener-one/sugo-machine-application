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

#include <azmq/socket.hpp>
#include <chrono>
#include <memory>

#include "Common/IOContext.hpp"
#include "MessageBroker/Message.hpp"
#include "MessageBroker/StreamBuffer.hpp"

namespace sugo::message_broker
{
/// @brief Class representing a client socket
class ClientSocket : public azmq::req_socket
{
public:
    // TODO Use boost signle thread optimization
    explicit ClientSocket(boost::asio::io_context& context);
};

/**
 * @brief Class for sending messages.
 * @todo This class should use the azmq::message approach for sending.
 *
 */
class Client
{
public:
    explicit Client(common::IOContext& ioContext);
    virtual ~Client();

    /// @brief Copy constructor.
    Client(const Client&) = delete;

    /// @brief Move constructor.
    Client(Client&&) = default;

    /// @brief Copy operator.
    Client& operator=(const Client&) = delete;

    /// @brief Move operator.
    Client& operator=(Client&&) = default;

    /**
     * @brief Sets the send and receive timeouts.
     *
     * @param address The address to connect to.
     * @param timeoutSend Receive timeout.
     * @param timeoutReceive Send timeout.
     */
    bool connect(const Address&                   address,
                 const std::chrono::milliseconds& timeoutSend    = std::chrono::milliseconds(-1),
                 const std::chrono::milliseconds& timeoutReceive = std::chrono::milliseconds(-1));

    /**
     * Disconnects from the last connected point.
     *
     * @return true if a connection was established and could be disconnected.
     */
    bool disconnect();

    /**
     * @brief Sends a request message to the defined address and waits for the response.
     *
     * @param outMessage      Message to be sent.
     * @param[out] inResponse The response which should be replied from the server.
     * @return                True if the message could be send and the response has been received.
     */
    bool send(const StreamBuffer& outMessage, StreamBuffer& inResponse);

    /**
     * @brief Indicates if a connection is established.
     *
     * @return true  If connection is established.
     * @return false If connection is not established.
     */
    bool isConnected() const
    {
        return !m_address.empty();
    }

private:
    Address      m_address;  ///< Client address.
    ClientSocket m_socket;   ///< Client socket.
};

}  // namespace sugo::message_broker
