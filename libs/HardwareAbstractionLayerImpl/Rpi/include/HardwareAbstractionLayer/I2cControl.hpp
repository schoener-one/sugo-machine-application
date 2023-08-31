
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

#include <string>

#include "HardwareAbstractionLayer/HalTypes.hpp"

namespace sugo::hal
{
/// @brief Control class for handling I2C messaging.
class I2cControl
{
public:
    /// @brief I2C address type.
    using Address = uint8_t;

    I2cControl() = default;
    ~I2cControl();

    bool init(const std::string& device);
    void finalize();

    bool read(Address address, const ByteBuffer& command, ByteBuffer& retValue) const;
    bool write(Address address, const ByteBuffer& command) const;

private:
    constexpr static int InvalidFileDescriptor = -1;

    int m_fd = InvalidFileDescriptor;
};
}  // namespace sugo::hal