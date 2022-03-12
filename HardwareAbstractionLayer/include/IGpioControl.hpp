///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2019-10-15
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IGpioPin.hpp"
#include "IHalObject.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for contolling GPIOs.
 *
 */
class IGpioControl : public IHalObject
{
public:
    /// GPIO pin map
    using GpioPinMap = IHalObject::Map<IGpioPin>;

    /// Infinite wait time for waitForChange.
    constexpr static int InfiniteTimeout = -1;

    /**
     * @brief Destroy the IGpioController object
     *
     */
    virtual ~IGpioControl()
    {
    }

    /**
     * @brief Returns a map of available GPIOs.
     *
     * @return const GpioPinMap& Map of available GPIOs.
     */
    virtual const GpioPinMap& getGpioMap() = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
