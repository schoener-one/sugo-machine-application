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

#include "IHalObject.hpp"
#include "UnitValue.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for ADC filter.
 *
 */
class IAdcFilter : public IHalObject
{
public:
    /// Raw value type
    using RawValueType = int32_t;
    /// Translated value type
    using ValueType = UnitValue<RawValueType>;

    /**
     * @brief Destroy the IAdcFilter object.
     *
     */
    virtual ~IAdcFilter()
    {
    }

    /**
     * @brief Returns the converted value
     *
     * @return ValueType Converted value
     */
    virtual ValueType convert(RawValueType rawValue) const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
