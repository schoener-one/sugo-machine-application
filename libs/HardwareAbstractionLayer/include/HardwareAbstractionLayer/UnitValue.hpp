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

namespace sugo::hal
{
/// @brief Unit types
enum class Unit
{
    Unknown = 0,
    Celcius,
    Volt,
    Ohm,
    Rpm
};

/**
 * @brief Class representing values which have an unit.
 *
 * @tparam ValueT Value type
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

    UnitValue(UnitValue<ValueT>&&) noexcept = default;

    UnitValue<ValueT>& operator=(const UnitValue<ValueT>& other) = default;

    UnitValue<ValueT>& operator=(UnitValue<ValueT>&& other) noexcept = default;

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
    bool operator==(const UnitValue<ValueT>& other) const
    {
        return (other.m_unit == m_unit) && (other.m_value == m_value);
    }

private:
    ValueT m_value;
    Unit   m_unit;
};

}  // namespace sugo::hal
