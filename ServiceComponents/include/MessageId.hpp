///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2020 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   22.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

namespace sugo
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
 * @tparam IdT         Message id type
 * @tparam ReceiverIdT Receiver id type
 */
template <typename IdT, typename ReceiverIdT>
class GenericCommandId : public GenericMessageId<IdT>
{
public:
    GenericCommandId(const IdT& id, const ReceiverIdT& receiverId)
        : GenericMessageId<IdT>(id), m_receiverId(receiverId)
    {
    }

    virtual ~GenericCommandId()                                 = default;
    GenericCommandId(const GenericCommandId<IdT, ReceiverIdT>&) = default;
    GenericCommandId(GenericCommandId<IdT, ReceiverIdT>&&)      = default;
    GenericCommandId<IdT, ReceiverIdT>& operator=(const GenericCommandId<IdT, ReceiverIdT>&) =
        default;
    GenericCommandId<IdT, ReceiverIdT>& operator=(GenericCommandId<IdT, ReceiverIdT>&&) = default;

    /**
     * @brief Returns the receiver id.
     *
     * @return Receiver id.
     */
    const ReceiverIdT& getReceiverId() const
    {
        return m_receiverId;
    }

protected:
    const ReceiverIdT m_receiverId;
};

/**
 * @brief Class represents a notification identifier.
 *
 * @tparam IdT         Message id type
 * @tparam ReceiverIdListT Receiver id type
 */
template <typename IdT, typename ReceiverIdListT>
class GenericNotificationId : public GenericMessageId<IdT>
{
public:
    /// @brief List of notification receiver ids.
    using ReceiverIdList = ReceiverIdListT;

    GenericNotificationId(const IdT& id, const ReceiverIdList& receiverIdList)
        : GenericMessageId<IdT>(id), m_receiverIdList(receiverIdList)
    {
    }

    virtual ~GenericNotificationId()                                          = default;
    GenericNotificationId(const GenericNotificationId<IdT, ReceiverIdListT>&) = default;
    GenericNotificationId(GenericNotificationId<IdT, ReceiverIdListT>&&)      = default;
    GenericNotificationId<IdT, ReceiverIdListT>& operator                     =(
        const GenericNotificationId<IdT, ReceiverIdListT>&) = default;
    GenericNotificationId<IdT, ReceiverIdListT>& operator                     =(
        GenericNotificationId<IdT, ReceiverIdListT>&&) = default;

    /**
     * @brief Returns the receiver id list.
     *
     * @return Receiver id list.
     */
    const ReceiverIdList& getReceiverIdList() const
    {
        return m_receiverIdList;
    }

protected:
    const ReceiverIdList m_receiverIdList;
};
}  // namespace sugo
