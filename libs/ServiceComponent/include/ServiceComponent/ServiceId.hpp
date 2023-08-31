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

#include "MessageBroker/Message.hpp"

namespace sugo::service_component
{
/**
 * @brief Base class for all service identifiers.
 * An service identifier specifies exactly one service peer.
 *
 * @tparam MessageIdT   Message identifier type.
 * @tparam AddressT     Address type.
 * @tparam TopicT       Topic name type.
 */
template <typename MessageIdT, typename AddressT, typename TopicT>
class GenericServiceId
{
public:
    constexpr GenericServiceId(const MessageIdT& id, AddressT address, TopicT topic)
        : m_id(id), m_address(std::move(address)), m_topic(std::move(topic))
    {
    }

    constexpr GenericServiceId(AddressT address, TopicT topic)
        : m_id(message_broker::Message::createIdentifier(address, topic)),
          m_address(std::move(address)),
          m_topic(std::move(topic))
    {
    }

    ~GenericServiceId()                                                         = default;
    GenericServiceId(const GenericServiceId<MessageIdT, AddressT, TopicT>&)     = default;
    GenericServiceId(GenericServiceId<MessageIdT, AddressT, TopicT>&&) noexcept = default;
    GenericServiceId<MessageIdT, AddressT, TopicT>& operator                    =(
        const GenericServiceId<MessageIdT, AddressT, TopicT>&) = default;
    GenericServiceId<MessageIdT, AddressT, TopicT>& operator                    =(
        GenericServiceId<MessageIdT, AddressT, TopicT>&&) noexcept = default;

    operator const MessageIdT&() const
    {
        return getMessageId();
    }

    /**
     * @brief Returns the message id.
     *
     * @return The message id.
     */
    const MessageIdT& getMessageId() const
    {
        return m_id;
    }

    /**
     * @brief Returns the address.
     *
     * @return The address value.
     */
    const AddressT& getAddress() const
    {
        return m_address;
    }

    /**
     * @brief Returns the subscription topic for the notification.
     *
     * @return The topic value.
     */
    const TopicT& getTopic() const
    {
        return m_topic;
    }

private:
    const MessageIdT m_id;
    const AddressT   m_address;
    const TopicT     m_topic;
};

// FIXME use BOOST_STRONG_TYPEDEF here!
/// @brief Type definition of command id.
using RequestId = GenericServiceId<message_broker::Message::Identifier, const char*, const char*>;

// FIXME use BOOST_STRONG_TYPEDEF here!
/// @brief Type definition of notification id.
using NotificationId =
    GenericServiceId<message_broker::Message::Identifier, const char*, const char*>;

/**
 * @brief Helper to print command ids.
 */
inline std::ostream& operator<<(std::ostream& ostr, const service_component::RequestId& requestId)
{
    return ostr << requestId.getAddress() << "/" << requestId.getTopic();
}
}  // namespace sugo::service_component
