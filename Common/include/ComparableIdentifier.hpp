///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2019 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2019-10-20
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>

namespace sugo
{
/**
 * Functor represents the comparator.
 */
template <class IdentifierT>
struct Comparator : std::binary_function<IdentifierT, IdentifierT, bool>
{
    bool operator()(const IdentifierT& id1, const IdentifierT& id2) const
    {
        return (id1 == id2);
    }
};

/**
 * @brief Functor represents a none comparator which always returns a defined value.
 *
 * @tparam IdentifierT Identifier type
 * @tparam retValue  Return value
 */
template <class IdentifierT, bool retValue = true>
struct NoneComperator : std::binary_function<IdentifierT, IdentifierT, bool>
{
    bool operator()(const IdentifierT&, const IdentifierT&) const
    {
        return retValue;
    }
};

/**
 * @brief Class represents a identity type.
 *
 * @tparam IdentifierT    Identifier type
 * @tparam ComparatorT  Identifier comparator
 */
template <class IdentifierT = int, class ComparatorT = Comparator<IdentifierT>>
class Ident
{
public:
    /**
     * Creates a new event object.
     *
     * @param id Identifier of the event.
     */
    Ident(IdentifierT id) : m_id(id)
    {
    }

    /**
     * Returns the Id of the event.
     * @return
     */
    const IdentifierT& getId() const
    {
        return m_id;
    }

    /**
     * Compares this event object to another one.
     *
     * @param other Other event object to be compared.
     * @return true if both have the same id, otherwise false.
     */
    bool operator==(const Ident& other) const
    {
        return operator==(other.getId());
    }

    /**
     * @brief Creates an order based on the identifier.
     *
     * @param other
     * @return true
     * @return false
     */
    bool operator<(const Ident& other) const
    {
        return (getId() < other.getId());
    }

    /**
     * Compares this object to the passed id.
     *
     * @param id Id to be compared.
     * @return true if the id is equal to own id, otherwise false.
     */
    bool operator==(const IdentifierT& id) const
    {
        ComparatorT compare;
        return compare(m_id, id);
    }

    /**
     * Converts the object to the identifier type.
     *
     * @return Event identifier type.
     */
    operator IdentifierT() const
    {
        return m_id;
    }

private:
    IdentifierT m_id;
};

/**
 * Class represents any events.
 */
template <class IdentifierT = int>
class AnyIdent : public Ident<IdentifierT, NoneComperator<IdentifierT>>
{
public:
    /**
     * Creates a new object.
     */
    AnyIdent() : Ident<>(0)
    {
    }
};

}  // namespace sugo
