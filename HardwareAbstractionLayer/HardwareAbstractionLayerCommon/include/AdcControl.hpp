///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "HalObject.hpp"
#include "IAdcControl.hpp"
#include "IConfiguration.hpp"

namespace sugo::hal
{
/**
 * @brief Class for contolling ADC input devices
 *
 */
class AdcControl : public HalObject, public IAdcControl
{
public:
    /**
     * @brief Construct a new ADC Control object
     *
     * @param id Identifier of that object
     */
    AdcControl(const Identifier& id) : HalObject(id) {}

    /**
     * @brief Initializes the object.
     *
     * @param configuration Configuration to be applied for initialization
     * @return true  If the object could be initialized successfully
     * @return false If the object could not be initialized
     */
    bool init(const IConfiguration& configuration);

    const AdcInputMap& getAdcInputMap() override { return m_adcInputMap; }

private:
    AdcInputMap m_adcInputMap;
};

}  // namespace sugo::hal
