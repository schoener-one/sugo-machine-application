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

#include "HardwareAbstractionLayer/IHalObject.hpp"
#include "HardwareAbstractionLayer/UnitValue.hpp"

namespace sugo::hal
{
/// @brief Interface class for analog input devices.
class ITemperatureSensor : public IHalObject
{
public:
    /// Raw value type
    using RawTemperature = int32_t;
    /// Translated value type
    using Temperature = UnitValue<RawTemperature>;

    /**
     * @brief Returns a unit translated value according to the used filter.
     *
     * @return Temperature Unit translated value.
     */
    virtual Temperature getTemperature() const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
