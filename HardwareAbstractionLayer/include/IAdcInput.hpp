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

#include "IAdcFilter.hpp"
#include "IHalObject.hpp"
#include "UnitValue.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for analog input devices.
 *
 */
class IAdcInput : public IHalObject
{
public:
    /**
     * @brief Destroy the IAdcInput object.
     *
     */
    virtual ~IAdcInput()
    {
    }

    /**
     * @brief Returns the raw ADC value
     *
     * @return IAdcFilter::AdcValueType Raw ADC value
     */
    virtual IAdcFilter::RawValueType getRawValue() const = 0;

    /**
     * @brief Returns a unit translated value according to the used filter.
     *
     * @return IAdcFilter::ValueType Unit translated value.
     */
    virtual IAdcFilter::ValueType getValue() const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
