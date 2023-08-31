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

#pragma once

#include <limits>

#include "HardwareAbstractionLayer/IGpioControl.hpp"
#include "HardwareAbstractionLayer/ITemperatureSensor.hpp"

namespace sugo::hal
{
class SpiControl;
class Max31865;

/// @brief Class represents an ADC channel.
class TemperatureSensor : public ITemperatureSensor
{
public:
    TemperatureSensor(const Identifier& id, SpiControl& spiControl,
                      const IGpioControl::GpioPinMap& gpioPins)
        : ITemperatureSensor(id), m_spiControl(spiControl), m_gpioPins(gpioPins)
    {
    }
    ~TemperatureSensor() override;

    void finalize();
    bool init(const common::IConfiguration& configuration) override;

    Temperature getTemperature() const override;

private:
    SpiControl&                     m_spiControl;
    const IGpioControl::GpioPinMap& m_gpioPins;
    Max31865*                       m_driver = nullptr;
};

}  // namespace sugo::hal
