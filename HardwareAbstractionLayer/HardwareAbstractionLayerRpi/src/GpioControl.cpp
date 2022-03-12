///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#include "GpioControl.hpp"
#include "GpioPin.hpp"
#include "HalHelper.hpp"

using namespace sugo::hal;

bool GpioControl::init(const IConfiguration& configuration)
{
    return initEnabledSubComponents<IGpioPin, GpioPin>(configuration, "pin", m_gpioPinMap);
}
