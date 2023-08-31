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

#include <boost/asio/buffer.hpp>

#include "Common/Logger.hpp"
#include "Common/Types.hpp"
#include "MessageBroker/Publisher.hpp"

using namespace sugo::message_broker;
using namespace sugo::common;

PublisherSocket::PublisherSocket(boost::asio::io_context& ioContext) : azmq::pub_socket(ioContext)
{
}

Publisher::Publisher(std::string address, common::IOContext& ioContext)
    : m_address(std::move(address)), m_socket(ioContext.getContext())
{
}

Publisher::~Publisher()  // NOLINT(modernize-use-equals-default) -
                         // std::unique_ptr/std::default_delete sizeof unknown
{
}

bool Publisher::publish(const Topic& topic, const StreamBuffer& message)
{
    boost::system::error_code ec;
    // Send topic first
    size_t sentBytes = m_socket.send(boost::asio::buffer(topic), ZMQ_SNDMORE, ec);

    if (ec)
    {
        LOG(error) << "Failed to send publish topic: " << ec.message();
        return false;
    }
    else if (sentBytes == 0)
    {
        LOG(error) << "Failed to send publish topic: no bytes sent";
        return false;
    }

    // Send message data afterwards
    sentBytes = m_socket.send(message.data(), ZMQ_DONTWAIT, ec);

    if (ec)
    {
        LOG(error) << "Failed to publish message: " << ec.message();
        return false;
    }
    else if (sentBytes == 0)
    {
        LOG(error) << "Failed to send publish topic: no bytes sent";
        return false;
    }

    LOG(trace) << "Published " << sentBytes << " bytes";

    return true;
}

bool Publisher::start()
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
    return m_isRunning;
}

void Publisher::stop()
{
    if (!isRunning())
    {
        return;
    }

    boost::system::error_code ec;
    m_socket.unbind(m_address, ec);

    if (ec)
    {
        LOG(error) << "Failed unbind from address: " << ec.message();
    }

    m_isRunning = false;
}
