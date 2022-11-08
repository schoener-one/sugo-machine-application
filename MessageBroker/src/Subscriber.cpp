///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis
 * @date:   16.10.2022
 */
///////////////////////////////////////////////////////////////////////////////

#include "Subscriber.hpp"
#include "Logger.hpp"

#include <algorithm>
#include <azmq/socket.hpp>

namespace sugo::message
{
class SubscriberSocket : public azmq::sub_socket
{
public:
    explicit SubscriberSocket(boost::asio::io_context& context) : azmq::sub_socket(context)
    {
    }
};
}  // namespace sugo::message

using namespace sugo::message;

Subscriber::Subscriber(IMessageHandler& messageHandler, IOContext& ioContext)
    : m_messageHandler(messageHandler)
{
    for (auto& item : m_socketInfoArray)
    {
        item.socket = std::make_unique<SubscriberSocket>(ioContext.getContext());
    }
}

Subscriber::~Subscriber()
{
}

bool Subscriber::subscribe(const Address& address, const MessageId& topic)
{
    assert(!address.empty());

    if (m_subscriptionMap.count(topic) != 0)
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
        socketData->socket->connect(address, ec);

        if (ec)
        {
            LOG(error) << "Failed to connect to " << address << ": " << ec.message();
            return false;
        }

        socketData->address = address;
        isNewConnection     = true;
    }

    boost::system::error_code ec;
    socketData->socket->set_option(azmq::socket::subscribe(topic), ec);

    if (ec)
    {
        LOG(error) << "Failed to subscribe to " << topic << ": " << ec.message();

        if (isNewConnection)
        {
            // only if we created the first connection we have to clear it completely
            socketData->socket->disconnect(address, ec);
            socketData->address.clear();
        }
        return false;
    }

    m_subscriptionMap.emplace(topic, socketData);

    if (isNewConnection)
    {
        receiveNotification(*socketData, true);
    }

    return true;
}

bool Subscriber::unsubscribe(const MessageId& topic)
{
    auto iter = m_subscriptionMap.find(topic);

    if (iter == m_subscriptionMap.end())
    {
        LOG(error) << "Failed to unsubscribe from " << topic << ": unknown subscription";
        return false;
    }

    assert(static_cast<size_t>(std::distance(m_socketInfoArray.begin(), iter->second)) <
           m_socketInfoArray.size());
    auto& socketData = *(iter->second);

    socketData.socket->set_option(azmq::socket::unsubscribe(topic));
    m_subscriptionMap.erase(topic);

    if (getSubscriptionCount(iter->second) == 0)
    {
        // No more subscriptions for that address
        boost::system::error_code ec;
        LOG(trace) << "Disconnecting from address: " << socketData.address;
        socketData.socket->disconnect(socketData.address, ec);

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
    socketData.socket->async_receive_more(
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
    StreamBuffer dummyBuffer;
    if (!m_messageHandler.processReceived(receiveBuf, dummyBuffer))
    {
        LOG(error) << "Failed to process received message";
        return false;
    }

    // Do a post process here to decouple message response from internal processing!
    m_messageHandler.processPost();
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