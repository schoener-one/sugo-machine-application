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

#include <atomic>
#include <cassert>
#include <map>
#include <memory>
#include <mutex>

#include "Common/IOContext.hpp"
#include "MessageBroker/Client.hpp"
#include "MessageBroker/IMessageBroker.hpp"
#include "MessageBroker/Publisher.hpp"
#include "MessageBroker/Server.hpp"
#include "MessageBroker/Subscriber.hpp"

namespace sugo::message_broker
{
/**
 * Class which handles messages.
 *
 */
class MessageBroker : public IMessageBroker
{
public:
    /// @brief Service types
    enum class Service
    {
        Responder,  ///< Responder which receives requests and sends the response
        Publisher,  ///< Publisher which publishes notifications
        Subscriber  ///< Subscriber which could subscribe to a notification publisher
    };

    /// @brief Maximum time to transmit a message (inclusive response).
    inline static constexpr std::chrono::milliseconds MaxMessageTransmissionTime{50};

    /**
     * @brief Construct a new command message broker object
     *
     * @param address   Address of this broker instance.
     * @param ioContext Io context to be used for receiving messages.
     */
    MessageBroker(const Address& address, common::IOContext& ioContext);

    /// @brief Default destructor.
    ~MessageBroker() override = default;

    /// @brief Copy constructor.
    MessageBroker(const MessageBroker&) = delete;

    /// @brief Move constructor.
    MessageBroker(MessageBroker&& messageBroker) = delete;

    /// @brief Copy assignment operator.
    MessageBroker& operator=(const MessageBroker&) = delete;

    /// @brief Move assignment operator.
    MessageBroker& operator=(MessageBroker&& messageBroker) = delete;

    bool notify(Message& message, const Topic& topic) override;

    bool subscribe(const Address& publisherAddress, const Topic& topic) override;

    bool send(Message& message, const Address& receiverAddress, ResponseMessage& response) override;

    bool unsubscribe(const Address& publisherAddress, const Topic& topic) override;

    bool start() override;

    void stop() override;

    bool isRunning() const override
    {
        return m_server.isRunning() && m_publisher.isRunning() && m_ioContext.isRunning();
    }

    /**
     * @brief Creates a full qualified address based on the base address.
     * For inproc communication: inproc://<base-address>[Pub]
     * For tcp communication:    tcp://<base-address>:<service-port>
     *
     * @param address Address without an prefix.
     * @param serviceType Type of the service for which this address is been used.
     * @return        The full qualified address.
     */
    static Address createFullQualifiedAddress(const Address& address, Service serviceType);

    void registerRequestMessageHandler(const Message::Identifier& messageId,
                                       RequestMessageHandler&     handler) override
    {
        m_requestHandlers[messageId] = handler;
    }

    void registerRequestMessageHandler(const Message::Identifier& messageId,
                                       RequestMessageHandler&&    handler) override
    {
        m_requestHandlers[messageId] = handler;
    }

    void registerNotificationMessageHandler(const Message::Identifier&  messageId,
                                            NotificationMessageHandler& handler) override
    {
        m_notificationHandlers[messageId] = handler;
    }

    void registerNotificationMessageHandler(const Message::Identifier&   messageId,
                                            NotificationMessageHandler&& handler) override
    {
        m_notificationHandlers[messageId] = handler;
    }

    void unregisterMessageHandler(const Message::Identifier& messageId) override;

    bool hasRegisteredMessageHandler(const Message::Identifier& messageId) const override;

private:
    /**
     * @brief Processes received request messages.
     *
     * @param in Input stream buffer to read received messages from.
     * @param out Output stream buffer to write response messages to.
     * @return true If the message could be processed successfully.
     * @return false If the message could not be process successfully.
     */
    bool processReceivedRequestMessage(StreamBuffer& in, StreamBuffer& out);

    /**
     * @brief Processes received notification messages.
     *
     * @param in Input stream buffer to read received messages from.
     * @return true If the message could be processed successfully.
     * @return false If the message could not be process successfully.
     */
    bool processReceivedNotificationMessage(StreamBuffer& in);

    /// @brief Request message handler map type.
    using RequestMessageHandlerMap = std::map<Message::Identifier, RequestMessageHandler>;
    /// @brief Notification message handler map type.
    using NotificationMessageHandlerMap = std::map<Message::Identifier, NotificationMessageHandler>;

    /**
     * @brief Increment and returns the next sequence number.
     *
     * @return The next sequence number.
     */
    uint32_t getNextSequenceNumber()
    {
        return m_sequenceNumber++;
    }

    Server                        m_server;      ///< Server instance
    Client                        m_client;      ///< Client instance
    Publisher                     m_publisher;   ///< Publisher instance
    Subscriber                    m_subscriber;  ///< Subscriber instance
    common::IOContext&            m_ioContext;   ///< Io context of the client and server instances.
    RequestMessageHandlerMap      m_requestHandlers;       ///< Request message handler map.
    NotificationMessageHandlerMap m_notificationHandlers;  ///< Notification message handler map.
    std::mutex           m_mutexClient;       ///< Mutex to restrict concurrent usage of the client.
    std::atomic_uint32_t m_sequenceNumber{};  ///< Sequence number of the next sent message.
};

}  // namespace sugo::message_broker
