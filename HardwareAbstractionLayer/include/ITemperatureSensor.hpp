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
    using RawTemperature = int32_t;
    /// Translated value type
    using Temperature = UnitValue<RawTemperature>;

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
     * @return Temperature Unit translated value.
     */
    virtual Temperature getTemperature() const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
