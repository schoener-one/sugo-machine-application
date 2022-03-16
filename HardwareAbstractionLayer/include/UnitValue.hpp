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

namespace sugo::hal
{
/**
 * @brief Unit types
 *
 */
enum class Unit
{
    MilliVolt,
    Celcius,
    Degree
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

    ValueT getValue()
    {
        return m_value;
    }

    Unit getUnit()
    {
        return m_unit;
    }

    operator ValueT()
    {
        return m_value;
    }

    operator Unit()
    {
        return m_unit;
    }

private:
    ValueT     m_value;
    const Unit m_unit;
};

}  // namespace sugo::hal
