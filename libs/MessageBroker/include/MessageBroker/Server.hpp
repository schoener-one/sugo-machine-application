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

#include <array>
#include <azmq/socket.hpp>
#include <functional>
#include <istream>

#include "Common/IOContext.hpp"
#include "Common/IRunnable.hpp"
#include "MessageBroker/IServer.hpp"
#include "MessageBroker/Message.hpp"
#include "MessageBroker/StreamBuffer.hpp"

namespace sugo::message_broker
{
class ServerSocket : public azmq::rep_socket
{
public:
    // TODO Use boost signle thread optimization
    explicit ServerSocket(boost::asio::io_context& ioContext);
};

/// @brief Class which represents a server to handle request messages.
class Server : public IServer
{
public:
    /**
     * @brief Callback function to process a new received request message.
     *
     * @param      receivedMessage Input stream buffer as received message.
     * @param[out] responseMessage Output stream buffer to write the response to.
     * Every request message needs a response message as a reply.
     * @return true if the message could be processed.
     *
     * @todo Change the interface to receive direct IMessage objects!
     */
    using RequestMessageHandler =
        std::function<bool(StreamBuffer& receivedMessage, StreamBuffer& responseMessage)>;
    /**
     * Constructor
     * @param address Address to bind to as listener.
     * @param messageHandler Instance to handle received messages.
     * @param ioContext IO context.
     */
    Server(Address address, RequestMessageHandler messageHandler, common::IOContext& ioContext);
    ~Server() override;

    /// @brief Copy constructor.
    Server(const Server&) = delete;

    /// @brief Move constructor.
    Server(Server&&);

    /// @brief Copy operator.
    Server& operator=(const Server&) = delete;

    /// @brief Move operator.
    Server& operator=(Server&&);

    /**
     * Returns the binded address.
     * @return Binded address
     */
    const Address& getAddress() const override
    {
        return m_address;
    }

    bool start() override;

    void stop() override;

    bool isRunning() const override
    {
        return m_isRunning;
    }

private:
    bool sendResponse(const StreamBuffer& sendBuffer);
    void receiveRequest();
    bool handleReceived(StreamBuffer& receiveBuf);

    const std::string     m_address;
    RequestMessageHandler m_messageHandler;
    ServerSocket          m_socket;
    bool                  m_isRunning = false;
    StreamBuffer          m_receiveBuffer;
};

}  // namespace sugo::message_broker
