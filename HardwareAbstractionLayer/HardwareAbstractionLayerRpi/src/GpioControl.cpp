///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#include <gpiod.hpp>
#include <unistd.h>

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
    if (m_chip != nullptr)
    {
        finalize();
    }

    auto deviceName = configuration.getOption("device").get<std::string>();
    LOG(debug) << getId() << ": open device '" << deviceName << "'";
    m_chip = new gpiod::chip(deviceName);

    if (!m_chip or m_chip->num_lines() == 0)
    {
        LOG(error) << getId() << ": failed to open device";
        finalize();
        return false;
    }

    return initEnabledSubComponents<IGpioPin, GpioPin, gpiod::chip>(configuration, "gpio-pin", m_gpioPinMap, *m_chip);
}

void GpioControl::finalize()
{
    m_gpioPinMap.clear();
    
    if (m_chip != nullptr)
    {
        delete m_chip;
        m_chip = nullptr;
    }
}