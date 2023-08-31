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

#include "HardwareAbstractionLayer/IGpioPin.hpp"
#include "HardwareAbstractionLayer/IHalObject.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for contolling GPIOs.
 *
 */
class IGpioControl : public IHalObject
{
public:
    /// GPIO pin map
    using GpioPinMap = IHalObject::Map<IGpioPin>;

    /// Infinite wait time for waitForChange.
    constexpr static int InfiniteTimeout = -1;

    /**
     * @brief Returns a map of available GPIOs.
     *
     * @return const GpioPinMap& Map of available GPIOs.
     */
    virtual const GpioPinMap& getGpioPinMap() = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
