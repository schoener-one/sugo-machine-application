/*
 * Event.hpp
 *
 *  Created on: 20.10.2019
 *      Author: denis
 */

#ifndef IDENT_HPP_
#define IDENT_HPP_

namespace sugo
{
/**
 * Functor represents the comparator.
 */
template <class IdT = int>
struct Comparator
{
    bool operator()(const IdT id1, const IdT id2) const
    {
        return (id1 == id2);
    }
};

/**
 * Functor represents a none comparator which always returns a defined value.
 */
template <class IdT = int, bool retValue = true>
struct NoneComperator
{
    bool operator()(const IdT, const IdT) const
    {
        return retValue;
    }
};

/**
 * Class represents an identifier type.
 */
template <class IdT = int, class ComparatorT = Comparator<IdT>>
class Ident
{
public:
    /**
     * Creates a new event object.
     *
     * @param id Identifier of the event.
     */
    explicit Ident(IdT id) : m_id(id)
    {
    }

    /**
     * Returns the Id of the event.
     * @return
     */
    IdT getId() const
    {
        return m_id;
    }

    /**
     * Compares this event object to another one.
     *
     * @param other Other event object to be compared.
     * @return true if both have the same id, otherwise false.
     */
    bool operator==(const Ident &other) const
    {
        return operator==(other.getId());
    }

    /**
     * Compares this object to the passed id.
     *
     * @param id Id to be compared.
     * @return true if the id is equal to own id, otherwise false.
     */
    bool operator==(IdT id) const
    {
        Comparator<> compare;
        return compare(m_id, id);
    }

    /**
     * Converts the object to the identifier type.
     *
     * @return Event identifier type.
     */
    operator IdT() const
    {
        return m_id;
    }

private:
    IdT m_id;
};

/**
 * Class represents any events.
 */
template <class IdT = int>
class AnyIdent : public Ident<IdT, NoneComperator<IdT>>
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

#endif /* IDENT_HPP_ */
