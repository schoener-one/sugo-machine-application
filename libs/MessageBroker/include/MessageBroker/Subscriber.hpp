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
#include <functional>
#include <map>
#include <vector>

#include "Common/IOContext.hpp"
#include "MessageBroker/Message.hpp"
#include "MessageBroker/StreamBuffer.hpp"

namespace sugo::message_broker
{
/// @brief Class represents a subscriber socket.
class SubscriberSocket : public azmq::sub_socket
{
public:
    // TODO Use boost signle thread optimization
    explicit SubscriberSocket(boost::asio::io_context& ioContext);
};

/**
 * @brief Class represents a message subscriber.
 *
 */
class Subscriber
{
public:
    /**
     * @brief Called to process a new received notification message.
     *
     * @param      receivedMessage Input stream buffer as received message.
     * @return true if the message could be processed.
     */
    using NotifictionMessageHandler = std::function<bool(StreamBuffer& receivedMessage)>;

    /// @brief Sets the maximum allowed number of subscription sockets.
    static constexpr size_t MaxSubscriptionSockets = 8u;

    explicit Subscriber(NotifictionMessageHandler messageHandler, common::IOContext& ioContext);

    /// @brief Default destructor.
    virtual ~Subscriber() = default;

    /// @brief Copy constructor.
    Subscriber(const Subscriber&) = delete;

    /// @brief Move constructor.
    Subscriber(Subscriber&&) = default;

    /// @brief Copy operator.
    Subscriber& operator=(const Subscriber&) = delete;

    /// @brief Move operator.
    Subscriber& operator=(Subscriber&&) = default;

    /**
     * @brief Subscribe to a notification.
     *
     * @param address  Address to subscribe to.
     * @param topic    The topic to subscribe to. It must be unique across all topics subscribed by
     * this object.
     * @return true    If the subscription could be done successfully.
     * @return false   If the subscription could not be done successfully, i.e. because same
     * topic already exists.
     */
    bool subscribe(const Address& address, const Topic& topic);

    /**
     * @brief Unsubscribe from a notification.
     *
     * @param address  Address to unsubscribe from.
     * @param topic    The topic to unsubscribe from.
     * @return true    If the unsubscription could be done successfully.
     * @return false   If the unsubscription could not be done successfully.
     */
    bool unsubscribe(const Address& address, const Topic& topic);

private:
    /// @brief Class representing subscriber socket data
    struct SubscriberSocketData
    {
        SubscriberSocketData(Address address, SubscriberSocket socket, StreamBuffer receiveBuffer)
            : address(std::move(address)),
              socket(std::move(socket)),
              receiveBuffer(std::move(receiveBuffer))
        {
        }

        Address          address;        ///< Publisher connection address
        SubscriberSocket socket;         ///< Subscriber socket
        StreamBuffer     receiveBuffer;  ///< Receive buffer

        bool isConnected()
        {
            return !address.empty();
        }
    };

    using SubscriberSocketInfoArray = std::vector<SubscriberSocketData>;
    using SubscriptionMap = std::map<Message::Identifier, SubscriberSocketInfoArray::iterator>;

    void receiveNotification(SubscriberSocketData& socketData, bool expectMoreToReceive);
    bool handleReceived(StreamBuffer& receiveBuf);

    SubscriberSocketInfoArray::iterator findSocket(const Address& address);
    size_t                              getSubscriptionCount(
                                     const SubscriberSocketInfoArray::iterator& subscriberSocketInfoIter);

    NotifictionMessageHandler m_messageHandler;
    SubscriberSocketInfoArray m_socketInfoArray;
    SubscriptionMap           m_subscriptionMap;
};

}  // namespace sugo::message_broker
