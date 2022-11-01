///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-15
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>

namespace sugo::hal
{
/**
 * @brief Unit types
 *
 */
enum class Unit
{
    Unknown = 0,
    Celcius,
    Volt,
    Ohm,
    Rpm
};

/**
 * @brief Basic class for value with an unit.
 *
 * @tparam ValueT
 */
template <class ValueT = float>
class UnitValue
{
public:
    /**
     * @brief Construct a new Unit Value object
     *
     * @param value Initial value
     * @param unit  Unit of that value
     */
    UnitValue(ValueT value, Unit unit) : m_value(value), m_unit(unit)
    {
    }

    UnitValue(const UnitValue<ValueT>&) = default;

    UnitValue(UnitValue<ValueT>&&) = default;

    UnitValue<ValueT>& operator=(const UnitValue<ValueT>& other) = default;

    UnitValue<ValueT>& operator=(UnitValue<ValueT>&& other) = default;

    ValueT getValue() const
    {
        return m_value;
    }

    Unit getUnit() const
    {
        return m_unit;
    }

    operator ValueT() const
    {
        return m_value;
    }

    operator Unit() const
    {
        return m_unit;
    }

    /**
     * @brief Compares two unit values.
     * 
     * @param other 
     * @return true  If they are equal.
     * @return false If they are not equal.
     */
    bool operator== (const UnitValue<ValueT>& other) const
    {
        return (other.m_unit == m_unit) && (other.m_value == m_value);
    }

private:
    ValueT m_value;
    Unit   m_unit;
};

}  // namespace sugo::hal
