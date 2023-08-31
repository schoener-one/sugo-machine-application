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

#include <memory>
#include <ostream>
#include <string>
#include <type_traits>

#include <Message.pb.h>

#include "Common/Hash.hpp"
#include "Common/Types.hpp"
#include "MessageBroker/Protocol.hpp"

namespace sugo::message_broker
{
/// @brief Type definition of an address.
using Address = std::string;
/// @brief Type definition of a topic.
using Topic = std::string;

/// @brief Generic message type
/// @tparam ProtoMessageT Type of the underlying protobuf message type.
template <class ProtoMessageT>
class GenericMessage
{
public:
    /// @brief Message identifier type.
    using Identifier =
        uint32_t;  // typename
                   // std::decay<decltype(std::declval<ProtoMessageT>().header().id())>::type;

    static_assert(
        std::is_same<Identifier, typename std::decay<decltype(
                                     std::declval<ProtoMessageT>().header().id())>::type>::value);

    /**
     * @brief Creates a unique message identifier from address and topic.
     *
     * @param address Address of the message.
     * @param topic   Topic of the message.
     * @return Unique message identifier.
     */
    constexpr static inline Identifier createIdentifier(const char* address, const char* topic)
    {
        // FIXME concatenate address and topic and calc hash afterwards! C++17 doesn't support
        //       compile time string concatenation!
        return (common::Fnv1a<Identifier>::combine(common::Fnv1a<Identifier>::hash(address),
                                                   common::Fnv1a<Identifier>::hash(topic)));
    }

    /**
     * @brief Deserializes the message from an input stream
     *
     * @param in Input stream to deserialize the message from.
     * @return true If the message could be deserialized successfully.
     * @return false If the message could not be deserialized successfully.
     */
    bool deserialize(std::istream& in)
    {
        return m_message.ParseFromIstream(&in);
    }

    /**
     * @brief Serializes the message to an output stream
     *
     * @param out Output stream to serialize the message to.
     * @return true If the message could be serialized successfully.
     * @return false If the message could not be serialized successfully.
     */
    bool serialize(std::ostream& out)
    {
        return m_message.SerializeToOstream(&out);
    }

    /**
     * @brief Returns the message identifier.
     *
     * @return Identifier Identifier of the message.
     */
    Identifier getId() const
    {
        return m_message.header().id();
    }

    /**
     * @brief Sets the message identifier.
     *
     * @param id Message identifier value.
     */
    void setId(Identifier id)
    {
        m_message.mutable_header()->set_id(id);
    }

    /**
     * @brief Set the sequence number.
     *
     * @param sequenceNumber Sequence number to set.
     */
    void setSequence(uint32_t sequenceNumber)
    {
        m_message.mutable_header()->set_sequence(sequenceNumber);
    }

    /**
     * @brief Returns the message sequence number.
     *
     * @return Sequence number of the message.
     */
    uint32_t getSequence() const
    {
        return m_message.header().sequence();
    }

    /**
     * @brief Sets the message payload.
     *
     * @param payload Payload as a byte array.
     * @param size    Size of the byte array.
     */
    void setPayload(const std::byte* payload, std::size_t size)
    {
        m_message.set_payload(payload, size);
    }

    /**
     * @brief Sets the message payload.
     *
     * @param payload Payload as a string.
     */
    void setPayload(const std::string& payload)
    {
        m_message.set_payload(payload);
    }

    /**
     * @brief Sets the message payload.
     *
     * @param payload Payload as a string.
     */
    void setPayload(std::string&& payload)
    {
        m_message.set_payload(std::move(payload));
    }

    /**
     * @brief Sets the message payload as JSON string.
     *
     * @param payload Payload in JSON format.
     */
    void setPayload(const common::Json& payload)
    {
        setPayload(payload.dump());
    }

    /**
     * @brief Gets the message payload.
     *
     * @return Message payload as string.
     */
    const std::string& getPayload() const
    {
        return m_message.payload();
    }

protected:
    ProtoMessageT m_message;  ///< Underlying protobuf message object.
};

/// @brief Representing a message to be send and receive by the broker.
class Message : public GenericMessage<sugo::message_broker::proto::Message>
{
public:
    friend class ResponseMessage;  // for referTo()
};

/// @brief Represents a response message used as a reply for a received message.
class ResponseMessage : public GenericMessage<sugo::message_broker::proto::ResponseMessage>
{
public:
    /** @brief Response result type.
     * @note If a result message is received with a result type set to not Success,
     * also an error message is added!
     */
    enum class Result
    {
        /// Message successful processed.
        Success =
            sugo::message_broker::proto::ResponseMessage::Result::ResponseMessage_Result_Success,
        /// Request is not supported.
        UnsupportedRequest = sugo::message_broker::proto::ResponseMessage::Result::
            ResponseMessage_Result_UnsupportedRequest,
        /// Message format is invalid.
        InvalidMessage = sugo::message_broker::proto::ResponseMessage::Result::
            ResponseMessage_Result_InvalidMessage,
        /// Invalid payload received.
        InvalidPayload = sugo::message_broker::proto::ResponseMessage::Result::
            ResponseMessage_Result_InvalidPayload,
        /// General and unspecified error.
        Error = sugo::message_broker::proto::ResponseMessage::Result::ResponseMessage_Result_Error
    };

    /**
     * @brief Sets a result value.
     *
     * @param result Result value to be set.
     */
    void setResult(Result result)
    {
        m_message.set_result(
            static_cast<sugo::message_broker::proto::ResponseMessage_Result>(result));
    }

    /**
     * @brief Returns the result value.
     *
     * @return Result The set result value.
     */
    Result getResult() const
    {
        return static_cast<Result>(m_message.result());
    }

    /**
     * @brief Sets the detailed error description as JSON string.
     *
     * @param errorReason Error description in JSON format.
     */
    void setError(const common::Json& errorReason)
    {
        m_message.set_error(errorReason.dump());
    }

    /**
     * @brief Sets the detailed error description as JSON string.
     *
     * @param errorReason Raw error description string.
     */
    void setError(const std::string& errorReason)  // TODO move to ServiceComponent
    {
        const common::Json jsonErrorReason({{message_broker::id::ErrorReason, errorReason}});
        m_message.set_error(jsonErrorReason.dump());
    }

    /**
     * @brief Returns the detailed error description as JSON string.
     *
     * @return Error description in JSON format.
     */
    const std::string& getError() const
    {
        return m_message.error();
    }

    /**
     * @brief Translates the result value into a human readable name.
     *
     * @param result Result value to be translated.
     * @return Name of the result value.
     */
    static const std::string& getResultName(Result result)
    {
        return sugo::message_broker::proto::ResponseMessage::Result_Name(
            static_cast<sugo::message_broker::proto::ResponseMessage_Result>(result));
    }

    /**
     * @brief Sets the message header of the result to the same value as the message it refers to.
     *
     * @param message Message to which this response refers to.
     */
    void referTo(const Message& message)
    {
        *m_message.mutable_header() = message.m_message.header();
    }
};

/**
 * @brief Helper to print notification ids.
 * @param message Message to print
 */
std::ostream& operator<<(std::ostream& ostr, const Message& message);
}  // namespace sugo::message_broker