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

#include "HardwareAbstractionLayer/IGpioControl.hpp"

namespace gpiod
{
class chip;
}

namespace sugo::hal
{
/// @brief Class for contolling GPIOs.
class GpioControl : public IGpioControl
{
public:
    using IGpioControl::IGpioControl;

    ~GpioControl() override;

    bool init(const common::IConfiguration& configuration) override;

    void finalize();

    const GpioPinMap& getGpioPinMap() override
    {
        return m_gpioPinMap;
    }

private:
    gpiod::chip* m_device = nullptr;  ///< GPIO controller chip representation.
    GpioPinMap   m_gpioPinMap;        ///< GPIO pin map.
};

}  // namespace sugo::hal
