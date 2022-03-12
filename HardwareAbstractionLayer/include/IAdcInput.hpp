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

namespace sugo::hal
{
/**
 * @brief Interface class for analog input devices.
 *
 */
class IAdcInput : public IHalObject
{
public:
    /// Invalid input value
    constexpr static unsigned InvalidInput = 0xffffffff;

    /// Value type
    using AdcValueType = uint32_t;

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
     * @return AdcValueType
     */
    virtual AdcValueType getRawValue() const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
