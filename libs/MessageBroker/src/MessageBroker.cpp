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

#include <cassert>
#include <istream>
#include <ostream>

#include "Common/Timer.hpp"
#include "MessageBroker/MessageBroker.hpp"
#include "MessageBroker/MessageHelper.hpp"

using namespace sugo::message_broker;

namespace
{
/// @brief Address prefix
const std::string AddressPrefix{"inproc://"};
}  // namespace

Address MessageBroker::createFullQualifiedAddress(const Address& address, Service serviceType)
{
    std::string fullQualifiedAddress =
        AddressPrefix + address + ((serviceType == Service::Publisher) ? "Pub" : "");
    return fullQualifiedAddress;
}

MessageBroker::MessageBroker(const Address& address, common::IOContext& ioContext)
    : m_server(
          createFullQualifiedAddress(address, Service::Responder),
          [this](StreamBuffer& in, StreamBuffer& out) {
              return this->processReceivedRequestMessage(in, out);
          },
          ioContext),
      m_client(ioContext),
      m_publisher(createFullQualifiedAddress(address, Service::Publisher), ioContext),
      m_subscriber(
          [this](StreamBuffer& in) { return this->processReceivedNotificationMessage(in); },
          ioContext),
      m_ioContext(ioContext)
{
}

bool MessageBroker::start()
{
    assert(!m_ioContext.isRunning());
    if (!m_ioContext.start())
    {
        LOG(error) << "Failed to start io context";
        return false;
    }
    const bool success = m_publisher.start();
    return m_server.start() && success;
}

void MessageBroker::stop()
{
    m_server.stop();
    m_publisher.stop();
    m_ioContext.stop();
}

bool MessageBroker::send(Message& message, const Address& address, ResponseMessage& response)
{
    const Address fullAddress = createFullQualifiedAddress(address, Service::Responder);
    const std::lock_guard<std::mutex> lock(m_mutexClient);

    if (!m_client.connect(fullAddress, MaxMessageTransmissionTime, MaxMessageTransmissionTime))
    {
        LOG(error) << "Failed to connect to " << address;
        return false;
    }

    message.setSequence(getNextSequenceNumber());
    LOG(trace) << "Sending request message " << message << " to " << fullAddress;

    StreamBuffer outBuf;
    std::ostream outStream(&outBuf);

    if (!message.serialize(outStream))
    {
        LOG(error) << "Failed to serialize message";
        return false;
    }

    StreamBuffer inBuf;

    if (!m_client.send(outBuf, inBuf))
    {
        LOG(error) << "Failed to send message " << message << " to " << address;
        return false;
    }

    std::istream inStream(&inBuf);
    if (!response.deserialize(inStream))
    {
        LOG(error) << "Failed to parse response message";
        return false;
    }

    if (m_client.isConnected())
    {
        if (!m_client.disconnect())
        {
            LOG(error) << "Failed to disconnect";
            return false;
        }
    }

    return true;
}

bool MessageBroker::notify(Message& message, const Topic& topic)
{
    message.setSequence(getNextSequenceNumber());
    LOG(trace) << "Send notification message " << message << " from " << m_publisher.getAddress()
               << "/" << topic;

    StreamBuffer outBuf;
    std::ostream outStream(&outBuf);

    if (!message.serialize(outStream))
    {
        LOG(error) << "Failed to serialize message";
        return false;
    }

    return m_publisher.publish(topic, outBuf);
}

bool MessageBroker::subscribe(const Address& address, const Topic& topic)
{
    LOG(debug) << "Subscribing to " << address << "/" << topic;
    return m_subscriber.subscribe(createFullQualifiedAddress(address, Service::Publisher), topic);
}

bool MessageBroker::unsubscribe(const Address& address, const Topic& topic)
{
    LOG(debug) << "Unsubscribing from " << address << "/" << topic;
    return m_subscriber.unsubscribe(createFullQualifiedAddress(address, Service::Publisher), topic);
}

bool MessageBroker::processReceivedRequestMessage(StreamBuffer& inBuf, StreamBuffer& outBuf)
{
    Message      message;
    std::istream in(&inBuf);

    if (!message.deserialize(in))
    {
        LOG(error) << "Failed to parse message";
        return false;
    }

    LOG(trace) << "Received request message " << message;

    ResponseMessage response;
    std::ostream    out(&outBuf);

    if (m_requestHandler)
    {
        response = m_requestHandler(message);
        response.referTo(message);
    }
    else
    {
        LOG(error) << "Received unhandled message: " << message;
        response = createErrorResponseMessage(message, ResponseMessage::Result::UnsupportedRequest);
    }

    return response.serialize(out);
}

bool MessageBroker::processReceivedNotificationMessage(StreamBuffer& inBuf)
{
    Message      message;
    std::istream in(&inBuf);

    if (!message.deserialize(in))
    {
        LOG(error) << "Failed to parse message";
        return false;
    }

    LOG(trace) << "Received notification message " << message;

    if (m_notificationHandler)
    {
        m_notificationHandler(message);
        return true;
    }
    else
    {
        LOG(error) << "Received unhandled notification: " << message;
        return false;
    }
}
