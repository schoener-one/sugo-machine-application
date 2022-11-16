///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis
 * @date:   16.10.2022
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IMessageHandler.hpp"
#include "IOContext.hpp"
#include "Message.hpp"
#include "StreamBuffer.hpp"

#include <array>
#include <map>
#include <memory>

namespace sugo::message
{
class SubscriberSocket;

/**
 * @brief Class represents a message subscriber.
 *
 */
class Subscriber
{
public:
    /// @brief Sets the maximum allowed number of subscription sockets.
    static constexpr size_t MaxSubscriptionSockets = 8u;

    explicit Subscriber(IMessageHandler& messageHandler, IOContext& ioContext);
    virtual ~Subscriber() = default;

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
    bool subscribe(const Address& address, const MessageId& topic);

    /**
     * @brief Unsubscribe from a notification.
     *
     * @param address  Address to unsubscribe from.
     * @param topic    The topic to unsubscribe from.
     * @return true    If the unsubscription could be done successfully.
     * @return false   If the unsubscription could not be done successfully.
     */
    bool unsubscribe(const MessageId& topic);

private:
    struct SubscriberSocketData
    {
        ~SubscriberSocketData();

        Address                           address;        ///< Publisher connection address
        std::unique_ptr<SubscriberSocket> socket;         ///< Subscriber socket
        StreamBuffer                      receiveBuffer;  ///< Receive buffer

        bool isConnected()
        {
            return !address.empty();
        }
    };

    using SubscriberSocketInfoArray = std::array<SubscriberSocketData, MaxSubscriptionSockets>;
    using SubscriptionMap           = std::map<MessageId, SubscriberSocketInfoArray::iterator>;

    void receiveNotification(SubscriberSocketData& socketData, bool expectMoreToReceive);
    bool handleReceived(StreamBuffer& receiveBuf);

    SubscriberSocketInfoArray::iterator findSocket(const Address& address);
    size_t                              getSubscriptionCount(
                                     const SubscriberSocketInfoArray::iterator& subscriberSocketInfoIter);

    IMessageHandler&          m_messageHandler;
    SubscriberSocketInfoArray m_socketInfoArray;
    SubscriptionMap           m_subscriptionMap;
};

}  // namespace sugo::message
