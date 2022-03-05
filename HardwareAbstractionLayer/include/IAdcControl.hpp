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

#include <string>
#include <map>
#include <memory>

#include "IHalObject.hpp"
#include "IAdcInput.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for ADC input devices.
 *
 */
class IAdcControl : virtual public IHalObject
{
public:
    /// GPIO controller map
    using AdcInputMap = std::map<Identifier, std::unique_ptr<IAdcInput>>;

    /**
     * @brief Destroy the IAdcControl object.
     *
     */
    virtual ~IAdcControl() {}

    /**
     * @brief Returns a map of available ADC inputs
     *
     * @return const AdcInputMap& Map of available ADC inputs
     */
    virtual const AdcInputMap& getAdcInputMap() = 0;

protected:
    /**
     * @brief Construct a new IAdcControl object
     *
     */
    IAdcControl() {}
};

}  // namespace sugo::hal
