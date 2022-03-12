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

#include "IGpioControl.hpp"

namespace sugo::hal
{
/**
 * @brief Class for contolling GPIOs.
 *
 */
class GpioControl : public IGpioControl
{
public:
    using IGpioControl::IGpioControl;

    bool init(const IConfiguration& configuration) override;

    const GpioPinMap& getGpioMap() override
    {
        return m_gpioPinMap;
    }

private:
    GpioPinMap m_gpioPinMap;
};

}  // namespace sugo::hal
