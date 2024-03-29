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

#include <algorithm>

#include "Common/Logger.hpp"
#include "MessageBroker/Subscriber.hpp"

using namespace sugo::message_broker;

SubscriberSocket::SubscriberSocket(boost::asio::io_context& context) : azmq::sub_socket(context)
{
}

Subscriber::Subscriber(NotifictionMessageHandler messageHandler, common::IOContext& ioContext)
    : m_messageHandler(std::move(messageHandler))
{
    m_socketInfoArray.reserve(MaxSubscriptionSockets);  // TODO avoid dynamic memory allocation!
    for (std::size_t i = 0; i < MaxSubscriptionSockets; i++)
    {
        m_socketInfoArray.emplace_back(SubscriberSocketData(
            std::string(), SubscriberSocket(ioContext.getContext()), StreamBuffer()));
    }
}

bool Subscriber::subscribe(const Address& address, const Topic& topic)
{
    assert(!address.empty());
    const auto serviceId = Message::createIdentifier(address.c_str(), topic.c_str());

    if (m_subscriptionMap.count(serviceId) != 0)
    {
        LOG(error) << "Failed to subscribe to " << topic << ": subscription already exists";
        return false;
    }

    bool isNewConnection = false;
    auto socketData      = findSocket(address);

    if (socketData == m_socketInfoArray.end())
    {
        // No connection to publisher established
        socketData = findSocket(Address(""));  // find next empty one!

        if (socketData == m_socketInfoArray.end())
        {
            LOG(fatal) << "Failed to subscribe to " << topic << ": no more sockets available";
            return false;
        }

        LOG(trace) << "Connecting to address: " << address;
        boost::system::error_code ec;
        socketData->socket.connect(address, ec);

        if (ec)
        {
            LOG(error) << "Failed to connect to " << address << ": " << ec.message();
            return false;
        }

        socketData->address = address;
        isNewConnection     = true;
    }

    boost::system::error_code ec;
    socketData->socket.set_option(azmq::socket::subscribe(topic), ec);

    if (ec)
    {
        LOG(error) << "Failed to subscribe to " << topic << ": " << ec.message();

        if (isNewConnection)
        {
            // only if we created the first connection we have to clear it completely
            socketData->socket.disconnect(address, ec);
            socketData->address.clear();
        }
        return false;
    }

    m_subscriptionMap.emplace(serviceId, socketData);

    if (isNewConnection)
    {
        receiveNotification(*socketData, true);
    }

    return true;
}

bool Subscriber::unsubscribe(const Address& address, const Topic& topic)
{
    const auto serviceId = Message::createIdentifier(address.c_str(), topic.c_str());
    auto       iter      = m_subscriptionMap.find(serviceId);

    if (iter == m_subscriptionMap.end())
    {
        LOG(error) << "Failed to unsubscribe from " << address << "/" << topic
                   << ": unknown subscription";
        return false;
    }

    assert(static_cast<size_t>(std::distance(m_socketInfoArray.begin(), iter->second)) <
           m_socketInfoArray.size());
    auto& socketData = *(iter->second);

    socketData.socket.set_option(azmq::socket::unsubscribe(topic));
    m_subscriptionMap.erase(serviceId);

    if (getSubscriptionCount(iter->second) == 0)
    {
        // No more subscriptions for that address
        boost::system::error_code ec;
        LOG(trace) << "Disconnecting from address: " << socketData.address;
        socketData.socket.disconnect(socketData.address, ec);

        if (ec)
        {
            LOG(error) << "Failed to disconnect from " << socketData.address << ": "
                       << ec.message();
            return false;
        }
        socketData.address.clear();
    }

    return true;
}

void Subscriber::receiveNotification(SubscriberSocketData& socketData, bool expectMoreToReceive)
{
    socketData.socket.async_receive_more(
        socketData.receiveBuffer.prepare(StreamBuffer::MaxBufferSize),
        [&](boost::system::error_code ec, const azmq::socket::more_result_type moreResult) {
            const size_t bytesReceived = moreResult.first;
            const bool   moreToReceive = moreResult.second;
            LOG(trace) << "Received " << bytesReceived << " bytes"
                       << (moreToReceive ? " - expect more to receive" : "");

            if (!ec && !moreToReceive)
            {
                if (bytesReceived > 0)
                {
                    socketData.receiveBuffer.commit(bytesReceived);
                    (void)handleReceived(socketData.receiveBuffer);
                }
                else
                {
                    LOG(warning) << "Received empty buffer";
                }
            }
            else if (ec && !moreToReceive)  // ignore error in case more data is expected!
            {
                LOG(error) << "Receive error occurred: " << ec.message();
            }

            if (socketData.isConnected())
            {
                receiveNotification(socketData, !moreToReceive);
            }
        },
        (expectMoreToReceive) ? ZMQ_RCVMORE : 0);
}

bool Subscriber::handleReceived(StreamBuffer& receiveBuf)
{
    if (!m_messageHandler(receiveBuf))
    {
        LOG(error) << "Failed to process received message";
        return false;
    }

    return true;
}

Subscriber::SubscriberSocketInfoArray::iterator Subscriber::findSocket(const Address& address)
{
    return std::find_if(
        m_socketInfoArray.begin(), m_socketInfoArray.end(),
        [&address](const SubscriberSocketData& info) { return (info.address == address); });
}

size_t Subscriber::getSubscriptionCount(
    const SubscriberSocketInfoArray::iterator& subscriberSocketInfoIter)
{
    return std::count_if(m_subscriptionMap.begin(), m_subscriptionMap.end(),
                         [&subscriberSocketInfoIter](const SubscriptionMap::value_type& value) {
                             return (value.second == subscriberSocketInfoIter);
                         });
}