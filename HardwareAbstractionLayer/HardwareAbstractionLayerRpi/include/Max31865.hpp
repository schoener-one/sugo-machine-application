///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-04-04
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>

#include "HalTypes.hpp"
#include "IGpioPin.hpp"
#include "SpiControl.hpp"

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
