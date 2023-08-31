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

#include <azmq/socket.hpp>

#include "Common/Types.hpp"
#include "MessageBroker/Server.hpp"

using namespace sugo::message_broker;
namespace asio = boost::asio;

ServerSocket::ServerSocket(boost::asio::io_context& ioContext) : azmq::rep_socket(ioContext)
{
}

Server::Server(std::string address, RequestMessageHandler messageHandler,
               common::IOContext& ioContext)
    : m_address(std::move(address)),
      m_messageHandler(std::move(messageHandler)),
      m_socket(ioContext.getContext())
{
}

Server::~Server()  // NOLINT(modernize-use-equals-default) - std::unique_ptr/std::default_delete
                   // sizeof unknown
{
}

Server::Server(Server&& server)
    : m_address(std::move(server.m_address)),
      m_messageHandler(std::move(server.m_messageHandler)),
      m_socket(std::move(server.m_socket)),
      m_isRunning(std::move(server.m_isRunning))
{
}

Server& Server::operator=(Server&& server)
{
    *this = std::move(server);
    return *this;
}

bool Server::start()
{
    assert(!m_isRunning);
    boost::system::error_code ec;
    m_socket.bind(m_address, ec);

    if (ec)
    {
        LOG(error) << "Failed to bind address: " << ec.message();
        return false;
    }

    LOG(debug) << "Bind to address: " << m_address;
    m_isRunning = true;
    receiveRequest();
    return m_isRunning;
}

void Server::receiveRequest()
{
    m_socket.async_receive(
        m_receiveBuffer.prepare(StreamBuffer::MaxBufferSize),
        [&](boost::system::error_code ec, std::size_t bytesReceived) {
            LOG(trace) << "Received " << bytesReceived << " bytes";

            if (!ec)
            {
                if (bytesReceived > 0)
                {
                    m_receiveBuffer.commit(bytesReceived);
                    (void)handleReceived(m_receiveBuffer);
                }
                else
                {
                    LOG(warning) << "Received empty buffer";
                }
            }
            else
            {
                LOG(error) << "Receive error occurred: " << ec.message();
            }

            if (isRunning())
            {
                receiveRequest();
            }
        },
        ZMQ_DONTWAIT);
}

bool Server::handleReceived(StreamBuffer& receiveBuf)
{
    StreamBuffer sendBuffer;
    bool         success = m_messageHandler(receiveBuf, sendBuffer);

    if ((sendBuffer.size() > 0) && success)
    {
        // Just send if we have something to send!
        success = sendResponse(sendBuffer);
    }
    else
    {
        LOG(error) << "Failed to process received message"
                   << (((sendBuffer.size() == 0) ? ": response message is empty" : ""));
        success = false;
    }

    return success;
}

bool Server::sendResponse(const StreamBuffer& sendBuffer)
{
    boost::system::error_code ec;
    auto const                size = m_socket.send(sendBuffer.data(), 0, ec);

    if (ec || (size == 0))
    {
        LOG(error) << "Failed to send message: "
                   << (ec.failed() ? ec.message() : "response could not be sent");
        return false;
    }

    return true;
}

void Server::stop()
{
    if (isRunning())
    {
        boost::system::error_code ec;
        m_socket.unbind(m_address, ec);

        if (ec)
        {
            LOG(error) << "Failed unbind from address: " << ec.message();
        }

        m_isRunning = false;
    }
}
