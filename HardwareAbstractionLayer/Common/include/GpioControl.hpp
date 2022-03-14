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

namespace gpiod
{
class chip;
}

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

    ~GpioControl() override;

    bool init(const IConfiguration& configuration) override;

    const GpioPinMap& getGpioMap() override
    {
        return m_gpioPinMap;
    }

private:
    void finalize();

    gpiod::chip* m_chip = nullptr;
    GpioPinMap   m_gpioPinMap;
};

}  // namespace sugo::hal
