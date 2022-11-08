///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <gpiod.hpp>

#include "GpioControl.hpp"
#include "GpioPin.hpp"
#include "HalHelper.hpp"
#include "Logger.hpp"

using namespace sugo::hal;

GpioControl::~GpioControl()
{
    finalize();
}

bool GpioControl::init(const IConfiguration& configuration)
{
    if (m_device != nullptr)
    {
        finalize();
    }

    auto deviceName = configuration.getOption("device").get<std::string>();
    LOG(debug) << getId() << ": open device '" << deviceName << "'";
    m_device = new gpiod::chip(deviceName);

    if (!m_device or m_device->num_lines() == 0)
    {
        LOG(error) << getId() << ": failed to open device";
        finalize();
        return false;
    }

    return initEnabledSubComponents<IGpioPin, GpioPin, gpiod::chip>(configuration, "gpio-pin",
                                                                    m_gpioPinMap, *m_device);
}

void GpioControl::finalize()
{
    m_gpioPinMap.clear();

    if (m_device != nullptr)
    {
        delete m_device;
        m_device = nullptr;
    }
}