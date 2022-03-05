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

#include <string>
#include <map>
#include <memory>

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
    /// GPIO controller map
    using AdcInputMap = std::map<Identifier, std::unique_ptr<IAdcInput>>;

    /// Value type
    using AdcValueType = uint32_t;

    /**
     * @brief Destroy the IAdcInput object.
     *
     */
    virtual ~IAdcInput() {}

   /**
    * @brief Returns the raw ADC value
    * 
    * @return AdcValueType 
    */
    virtual AdcValueType getRawValue() const = 0;

protected:
    /**
     * @brief Construct a new IAdcInput object
     *
     */
    IAdcInput() {}
};

}  // namespace sugo::hal
