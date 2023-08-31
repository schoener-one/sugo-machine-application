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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <limits>

#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/HalHelper.hpp"
#include "HardwareAbstractionLayer/SpiControl.hpp"
#include "HardwareAbstractionLayer/TemperatureSensor.hpp"
#include "HardwareAbstractionLayer/TemperatureSensorControl.hpp"

using namespace sugo::hal;

TemperatureSensorControl::TemperatureSensorControl(const Identifier&               id,
                                                   const IGpioControl::GpioPinMap& gpioPins)
    : ITemperatureSensorControl(id), m_gpioPins(gpioPins)
{
}

TemperatureSensorControl::~TemperatureSensorControl()
{
    finalize();
}

bool TemperatureSensorControl::init(const common::IConfiguration& configuration)
{
    assert(m_spiControl == nullptr);

    const std::string device =
        std::string("/dev/") + configuration.getOption("device").get<std::string>();
    LOG(debug) << getId() << ": using SPI device '" << device << "'";

    m_spiControl = std::make_unique<SpiControl>();
    if (!m_spiControl->init(device))
    {
        LOG(error) << getId() << ": Failed to initialize SPI device: " << device;
        finalize();
        return false;
    }

    bool success = initEnabledSubComponents<ITemperatureSensor, TemperatureSensor, SpiControl&,
                                            const IGpioControl::GpioPinMap&>(
        configuration, id::TemperatureSensor, m_temperatureSensorMap, *m_spiControl, m_gpioPins);

    return success;
}

void TemperatureSensorControl::finalize()
{
    if (m_spiControl != nullptr)
    {
        m_spiControl.reset();
        m_spiControl = nullptr;
    }
    m_temperatureSensorMap.clear();
}
