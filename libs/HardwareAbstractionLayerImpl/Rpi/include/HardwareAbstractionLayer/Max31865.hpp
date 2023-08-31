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

#include <array>

#include "HardwareAbstractionLayer/HalTypes.hpp"
#include "HardwareAbstractionLayer/IGpioPin.hpp"
#include "HardwareAbstractionLayer/SpiControl.hpp"

namespace sugo::hal
{
/**
 * @brief
 *
 */
class Max31865
{
public:
    Max31865(SpiControl& spi, IGpioPin& ioCs) : m_spi(spi), m_ioCs(ioCs)
    {
    }

    bool init();

    /**
     * @brief Returns the most recent temperature value.
     *
     * @return int16_t Recent temperature value in Celcius.
     */
    int16_t getTemperature();

    /**
     * @brief Resets the chip to default mode.
     *
     * @return true If reset succeeded
     * @return false If reset failed
     */
    bool reset()
    {
        return init();
    }

private:
    bool readSpiData(uint8_t startRegister, ByteBuffer& readData);
    bool writeSpiRegister(uint8_t startRegister, const ByteBuffer& writeData);

    SpiControl& m_spi;
    IGpioPin&   m_ioCs;
};

}  // namespace sugo::hal
