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
#include "IConfiguration.hpp"
#include "IGpioControl.hpp"

namespace sugo::hal
{
/**
 * @brief Class for contolling GPIOs.
 *
 */
class GpioControl : public HalObject, public IGpioControl
{
public:
    /**
     * @brief Construct a new Gpio Control object
     *
     * @param id Identifier of that object
     */
    GpioControl(const Identifier& id) : HalObject(id) {}

    /**
     * @brief Initializes the object.
     *
     * @param configuration Configuration to be applied for initialization
     * @return true  If the object could be initialized successfully
     * @return false If the object could not be initialized
     */
    bool init(const IConfiguration& configuration);

    const GpioPinMap& getGpioMap() override { return m_gpioMap; }

private:
    GpioPinMap m_gpioMap;
};

}  // namespace sugo::hal
