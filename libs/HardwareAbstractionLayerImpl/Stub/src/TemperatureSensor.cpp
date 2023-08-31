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

#include "HardwareAbstractionLayer/TemperatureSensor.hpp"
#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/Simulator.hpp"

using namespace sugo::hal;

TemperatureSensor::~TemperatureSensor()
{
    finalize();
}

bool TemperatureSensor::init(const common::IConfiguration& configuration)
{
    const auto chipSelect = configuration.getOption(id::ChipSelect).get<std::string>();
    LOG(debug) << getId() << "." << id::ChipSelect << ": " << chipSelect;

    return true;
}

void TemperatureSensor::finalize()
{
}

TemperatureSensor::Temperature TemperatureSensor::getTemperature() const
{
    return Simulator::getInstance().getTemperature(getId());
}
