///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-04-04
 * @note: This implementation is based on Waveshare Github code.
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>

#include "IGpioPin.hpp"
#include "SpiControl.hpp"

namespace sugo::hal
{
/**
 * @brief
 *
 */
class AdcHat
{
public:
    enum DataRate
    {
        R2d5SPS = 0,
        R5SPS,
        R10SPS,
        R16d6SPS,
        R20SPS,
        R50SPS,
        R60SPS,
        R100SPS,
        R400SPS,
        R1200SPS,
        R2400SPS,
        R4800SPS,
        R7200SPS,
        R14400SPS,
        R19200SPS,
        R38400SPS,
    };

    enum Delay
    {
        D0s = 0,
        D8d7us,
        D17us,
        D35us,
        D169us,
        D139us,
        D278us,
        D555us,
        D1d1ms,
        D2d2ms,
        D4d4ms,
        D8d8ms,
    };

    enum Gain
    {
        G1  = 0, /*GAIN	1 */
        G2  = 1, /*GAIN	2 */
        G4  = 2, /*GAIN	4 */
        G8  = 3, /*GAIN	8 */
        G16 = 4, /*GAIN	16 */
        G32 = 5, /*GAIN	32 */
        G64 = 6, /*GAIN	64 */
    };

    /// Maximum number of supported channels.
    constexpr static uint32_t MaxChannels = 10u;
    using ValueList                       = std::array<uint32_t, MaxChannels>;

    AdcHat(SpiControl& spi, IGpioPin& ioCs, IGpioPin& ioRst, IGpioPin& ioRdy)
        : m_spi(spi), m_ioCs(ioCs), m_ioRst(ioRst), m_ioRdy(ioRdy)
    {
    }

    bool init(DataRate rate);

    uint32_t getChannelValue(uint8_t channel);
    void     getAll(ValueList& values);
    void     reset();

private:
    static void    delay(uint32_t ms);
    static uint8_t getChecksum(uint32_t val, uint8_t byte);
    bool           config(Gain gain, DataRate drate, Delay delay);
    void           writeSpiCmd(uint8_t cmd);
    void           writeSpiRegister(uint8_t reg, uint8_t data);
    uint8_t        readSpiData(uint8_t reg);
    uint8_t        readChipId();
    bool           setChannel(uint8_t channel);
    uint32_t       readData();

    SpiControl& m_spi;
    IGpioPin&   m_ioCs;
    IGpioPin&   m_ioRst;
    IGpioPin&   m_ioRdy;
};

}  // namespace sugo::hal
