///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-02-24
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include <memory>
#include <string>

#include "IAdcInput.hpp"
#include "IHalObject.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for ADC input devices.
 *
 */
class IAdcControl : public IHalObject
{
public:
    /// ADC input map
    using AdcInputMap = IHalObject::Map<IAdcInput>;

    /**
     * @brief Destroy the IAdcControl object.
     *
     */
    virtual ~IAdcControl()
    {
    }

    /**
     * @brief Returns a map of available ADC inputs
     *
     * @return const AdcInputMap& Map of available ADC inputs
     */
    virtual const AdcInputMap& getAdcInputMap() = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
