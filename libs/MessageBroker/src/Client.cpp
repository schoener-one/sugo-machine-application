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

#include <chrono>

#include "Common/Types.hpp"
#include "MessageBroker/Client.hpp"

using namespace sugo::message_broker;
namespace asio = boost::asio;

ClientSocket::ClientSocket(boost::asio::io_context& context) : azmq::req_socket(context)
{
}

Client::Client(common::IOContext& ioContext) : m_socket(ioContext.getContext())
{
}

Client::~Client()  // NOLINT(modernize-use-equals-default) - std::unique_ptr/std::default_delete
                   // sizeof unknown
{
}

bool Client::connect(const std::string& address, const std::chrono::milliseconds& timeoutSend,
                     const std::chrono::milliseconds& timeoutReceive)
{
    boost::system::error_code ec;
    LOG(trace) << "Connecting to address: " << address;
    m_socket.connect(address, ec);

    if (ec)
    {
        LOG(error) << "Failed to connect: " << ec.message();
        return false;
    }

    m_socket.set_option(azmq::socket::snd_timeo(static_cast<int>(timeoutSend.count())));
    m_socket.set_option(azmq::socket::rcv_timeo(static_cast<int>(timeoutReceive.count())));
    m_address = address;
    return true;
}

bool Client::disconnect()
{
    boost::system::error_code ec;

    if (!isConnected())
    {
        return false;
    }

    LOG(trace) << "Disconnecting from address: " << m_address;
    m_socket.disconnect(m_address, ec);
    m_address.clear();

    if (ec)
    {
        LOG(error) << "Failed to disconnect: " << ec.message();
        return false;
    }

    return true;
}

bool Client::send(const StreamBuffer& outMessage, StreamBuffer& inResponse)
{
    boost::system::error_code ec;
    auto                      size = m_socket.send(outMessage.data(), 0, ec);

    if (ec)
    {
        LOG(error) << "Failed to send message: " << ec.message();
        return false;
    }

    LOG(trace) << "Sent " << size << " bytes";

    if (size == 0)
    {
        LOG(warning) << "No data sent";
        return false;
    }

    size = m_socket.receive(inResponse.prepare(StreamBuffer::MaxBufferSize), 0, ec);

    if (ec)
    {
        LOG(error) << "Failed to receive response: " << ec.message();
        return false;
    }

    inResponse.commit(size);
    return true;
}
