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

#include <gmock/gmock.h>

#include "HardwareAbstractionLayer/IGpioControl.hpp"

namespace sugo::hal
{
/// @brief Mock class of IGpioControl
class IGpioControlMock : public IGpioControl
{
public:
    IGpioControlMock() : IGpioControl("IGpioControlMock")
    {
    }

    MOCK_METHOD(bool, init, (const common::IConfiguration&));
    MOCK_METHOD(const GpioPinMap&, getGpioPinMap, ());
};
}  // namespace sugo::hal
