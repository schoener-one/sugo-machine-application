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

#include <chrono>
#include <map>
#include <memory>

#include "IGpioPin.hpp"
#include "IHalObject.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for contolling GPIOs.
 *
 */
class IGpioControl : virtual public IHalObject
{
public:
    /// GPIO controller map
    using GpioPinMap = std::map<Identifier, std::unique_ptr<IGpioPin>>;

    /// Infinite wait time for waitForChange.
    constexpr static int InfiniteTimeout = -1;

    /**
     * @brief Destroy the IGpioController object
     *
     */
    virtual ~IGpioControl() {}

    /**
     * @brief Returns a map of available GPIOs.
     *
     * @return const GpioPinMap& Map of available GPIOs.
     */
    virtual const GpioPinMap& getGpioMap() = 0;

protected:
    IGpioControl() {}
};

}  // namespace sugo::hal
