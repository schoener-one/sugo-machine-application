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

#include <linux/spi/spidev.h>
#include <string>

namespace sugo::hal
{
/// @brief Control class for SPI interface
class SpiControl
{
public:
    enum SPIMode
    {
        Mode0,
        Mode1,
        Mode2,
        Mode3
    };

    enum SPIChipSelect
    {
        None,
        Low,
        High
    };

    enum SPIBitOrder
    {
        LsbFirst,
        MsbFirst
    };

    enum BusMode
    {
        Mode3Wire,
        Mode4Wire
    };

    SpiControl() = default;
    ~SpiControl();

    bool init(const std::string& device);
    void finalize(void);

    uint8_t writeByte(uint8_t buf);
    uint8_t readByte()
    {
        return writeByte(0x00);
    }

private:
    constexpr static int InvalidFileDescriptor = -1;

    bool setSpeed(uint32_t speed);
    int  setBusMode(BusMode mode);
    int  setBitOrder(SPIBitOrder Order);
    bool setChipSelect(SPIChipSelect CS_Mode);
    bool setMode(SPIMode mode);

    struct spi_ioc_transfer m_tr = {};
    int                     m_fd = InvalidFileDescriptor;
    uint16_t                m_mode;
};

}  // namespace sugo::hal
