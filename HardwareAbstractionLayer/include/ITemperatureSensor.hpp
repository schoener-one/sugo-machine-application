///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-07
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IHalObject.hpp"
#include "UnitValue.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for analog input devices.
 *
 */
class ITemperatureSensor : public IHalObject
{
public:
    /// Raw value type
    using RawValueType = int32_t;
    /// Translated value type
    using ValueType = UnitValue<RawValueType>;

    /**
     * @brief Destroy the ITemperatureSensor object.
     *
     */
    virtual ~ITemperatureSensor()
    {
    }

    /**
     * @brief Returns a unit translated value according to the used filter.
     *
     * @return ValueType Unit translated value.
     */
    virtual ValueType getTemperature() const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
