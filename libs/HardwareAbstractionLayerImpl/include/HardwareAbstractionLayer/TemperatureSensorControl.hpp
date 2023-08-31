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

#include <memory>

#include "HardwareAbstractionLayer/IGpioControl.hpp"
#include "HardwareAbstractionLayer/ITemperatureSensorControl.hpp"

namespace sugo::hal
{
class SpiControl;

/// @brief Class for contolling ADC input devices.
class TemperatureSensorControl : public ITemperatureSensorControl
{
public:
    TemperatureSensorControl(const Identifier& id, const IGpioControl::GpioPinMap& gpioPins);
    ~TemperatureSensorControl() override;

    bool init(const common::IConfiguration& configuration) override;

    void finalize();

    const TemperatureSensorMap& getTemperatureSensorMap() override
    {
        return m_temperatureSensorMap;
    }

private:
    const IGpioControl::GpioPinMap& m_gpioPins;
    TemperatureSensorMap            m_temperatureSensorMap;
    std::unique_ptr<SpiControl>     m_spiControl;
};

}  // namespace sugo::hal
