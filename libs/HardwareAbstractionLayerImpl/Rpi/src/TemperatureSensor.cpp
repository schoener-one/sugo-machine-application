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

#include <cassert>

#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/Max31865.hpp"
#include "HardwareAbstractionLayer/TemperatureSensor.hpp"

using namespace sugo::hal;

TemperatureSensor::~TemperatureSensor()
{
    finalize();
}

bool TemperatureSensor::init(const common::IConfiguration& configuration)
{
    assert(m_driver == nullptr);

    const std::string chipSelect = configuration.getOption(id::ChipSelect).get<std::string>();
    LOG(debug) << getId() << ": chip-select set to " << chipSelect;

    auto& ioCs = m_gpioPins.at(chipSelect);
    if (!ioCs)
    {
        LOG(error) << getId() << ": failed to get chip-select pin";
        return false;
    }

    m_driver = new Max31865(m_spiControl, *ioCs);
    if (!m_driver->init())
    {
        LOG(error) << getId() << ": failed to init sensor driver";
        finalize();
        return false;
    }

    return true;
}

void TemperatureSensor::finalize()
{
    if (m_driver != nullptr)
    {
        delete m_driver;
        m_driver = nullptr;
    }
}

TemperatureSensor::Temperature TemperatureSensor::getTemperature() const
{
    assert(m_driver != nullptr);

    return Temperature(m_driver->getTemperature(), Unit::Celcius);
}
