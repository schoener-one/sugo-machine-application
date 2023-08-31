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

#include "HardwareAbstractionLayer/GpioControl.hpp"
#include "HardwareAbstractionLayer/GpioPin.hpp"
#include "HardwareAbstractionLayer/HalHelper.hpp"

using namespace sugo::hal;

GpioControl::~GpioControl()  // NOLINT(modernize-use-equals-default)
{
}

bool GpioControl::init(const common::IConfiguration& configuration)
{
    return initEnabledSubComponents<IGpioPin, GpioPin>(configuration, id::GpioPin, m_gpioPinMap);
}

void GpioControl::finalize()
{
}