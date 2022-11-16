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

GpioControl::~GpioControl()  // NOLINT(modernize-use-equals-default)
{
}

bool GpioControl::init(const IConfiguration& configuration)
{
    return initEnabledSubComponents<IGpioPin, GpioPin>(configuration, "gpio-pin", m_gpioPinMap);
}

void GpioControl::finalize()
{
}