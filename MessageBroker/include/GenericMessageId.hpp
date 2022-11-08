///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2020 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   22.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace sugo::message
{
/**
 * @brief Base class for all message identifiers.
 * An message identifier consists of a message id.
 *
 * @tparam IdT       Message id
 */
template <typename IdT>
class GenericMessageId
{
public:
    GenericMessageId(const IdT& id) : m_id(id)
    {
    }

    virtual ~GenericMessageId()                    = default;
    GenericMessageId(const GenericMessageId<IdT>&) = default;
    GenericMessageId(GenericMessageId<IdT>&&)      = default;
    GenericMessageId<IdT>& operator=(const GenericMessageId<IdT>&) = default;
    GenericMessageId<IdT>& operator=(GenericMessageId<IdT>&&) = default;

    operator const IdT&() const
    {
        return getId();
    }

    /**
     * @brief Returns the message id.
     *
     * @return The message id.
     */
    const IdT& getId() const
    {
        return m_id;
    }

protected:
    const IdT m_id;
};

/**
 * @brief Class represents a command identifier.
 *
 * @tparam IdT              Message id type
 * @tparam ReceiverAddressT Receiver address type
 */
template <typename IdT, typename ReceiverAddressT>
class GenericCommandId : public GenericMessageId<IdT>
{
public:
    GenericCommandId(const IdT& id, const ReceiverAddressT& receiverAddress)
        : GenericMessageId<IdT>(id), m_receiverAddress(receiverAddress)
    {
    }

    virtual ~GenericCommandId()                                      = default;
    GenericCommandId(const GenericCommandId<IdT, ReceiverAddressT>&) = default;
    GenericCommandId(GenericCommandId<IdT, ReceiverAddressT>&&)      = default;
    GenericCommandId<IdT, ReceiverAddressT>& operator                =(
        const GenericCommandId<IdT, ReceiverAddressT>&) = default;
    GenericCommandId<IdT, ReceiverAddressT>& operator=(GenericCommandId<IdT, ReceiverAddressT>&&) =
        default;

    /**
     * @brief Returns the receiver address.
     *
     * @return Receiver address.
     */
    const ReceiverAddressT& getReceiverAddress() const
    {
        return m_receiverAddress;
    }

protected:
    const ReceiverAddressT m_receiverAddress;
};

/**
 * @brief Class represents a notification identifier.
 *
 * @tparam IdT               Message id type
 * @tparam PublisherAddressT Publisher address type
 */
template <typename IdT, typename PublisherAddressT>
class GenericNotificationId : public GenericMessageId<IdT>
{
public:
    GenericNotificationId(const IdT& id, const PublisherAddressT& publisherAddress)
        : GenericMessageId<IdT>(id), m_publisherAddress(publisherAddress)
    {
    }

    virtual ~GenericNotificationId()                                            = default;
    GenericNotificationId(const GenericNotificationId<IdT, PublisherAddressT>&) = default;
    GenericNotificationId(GenericNotificationId<IdT, PublisherAddressT>&&)      = default;
    GenericNotificationId<IdT, PublisherAddressT>& operator                     =(
        const GenericNotificationId<IdT, PublisherAddressT>&) = default;
    GenericNotificationId<IdT, PublisherAddressT>& operator                     =(
        GenericNotificationId<IdT, PublisherAddressT>&&) = default;

    /**
     * @brief Returns the publisher address according to the notification.
     *
     * @return The publisher address.
     */
    const PublisherAddressT& getPublisherAddress() const
    {
        return m_publisherAddress;
    }

protected:
    const PublisherAddressT m_publisherAddress;
};
}  // namespace sugo::message
