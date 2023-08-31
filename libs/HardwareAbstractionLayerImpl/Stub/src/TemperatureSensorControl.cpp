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

#include "HardwareAbstractionLayer/TemperatureSensorControl.hpp"
#include "HardwareAbstractionLayer/HalHelper.hpp"
#include "HardwareAbstractionLayer/TemperatureSensor.hpp"

namespace sugo::hal
{
class SpiControl
{
public:
    unsigned m_dummy = 0;
};
}  // namespace sugo::hal

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
    m_spiControl = std::make_unique<SpiControl>();
    return initEnabledSubComponents<ITemperatureSensor, TemperatureSensor, SpiControl&,
                                    const IGpioControl::GpioPinMap&>(
        configuration, id::TemperatureSensor, m_temperatureSensorMap, *m_spiControl, m_gpioPins);
}

void TemperatureSensorControl::finalize()
{
    m_spiControl.reset();
}