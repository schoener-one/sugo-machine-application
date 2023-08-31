///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <gpiod.hpp>

#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/GpioControl.hpp"
#include "HardwareAbstractionLayer/GpioPin.hpp"
#include "HardwareAbstractionLayer/HalHelper.hpp"

using namespace sugo::hal;

GpioControl::~GpioControl()
{
    finalize();
}

bool GpioControl::init(const common::IConfiguration& configuration)
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

    return initEnabledSubComponents<IGpioPin, GpioPin, gpiod::chip>(configuration, id::GpioPin,
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