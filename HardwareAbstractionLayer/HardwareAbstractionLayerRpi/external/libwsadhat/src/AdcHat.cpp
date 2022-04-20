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

#include "AdcHat.hpp"
#include "Logger.hpp"

#include <boost/format.hpp>
#include <unistd.h>

using namespace wsadhat;
using namespace sugo::hal;

// TODO Remove magic numbers

namespace
{
constexpr uint8_t  StatusBitADC1DataReady     = 0x40u;
constexpr unsigned MicrosecondsPerMillisecond = 1000u;

enum Reg
{
    /*Register address, followed by reset the default values */
    ID = 0,     // xxh
    POWER,      // 11h
    INTERFACE,  // 05h
    MODE0,      // 00h
    MODE1,      // 80h
    MODE2,      // 04h
    INPMUX,     // 01h
    OFCAL0,     // 00h
    OFCAL1,     // 00h
    OFCAL2,     // 00h
    FSCAL0,     // 00h
    FSCAL1,     // 00h
    FSCAL2,     // 40h
    IDACMUX,    // BBh
    IDACMAG,    // 00h
    REFMUX,     // 00h
    TDACP,      // 00h
    TDACN,      // 00h
    GPIOCON,    // 00h
    GPIODIR,    // 00h
    GPIODAT,    // 00h
    ADC2CFG,    // 00h
    ADC2MUX,    // 01h
    ADC2OFC0,   // 00h
    ADC2OFC1,   // 00h
    ADC2FSC0,   // 00h
    ADC2FSC1,   // 40h
};

enum Cmd
{
    RESET   = 0x06,  // Reset the ADC, 0000 011x (06h or 07h)
    START1  = 0x08,  // Start ADC1 conversions, 0000 100x (08h or 09h)
    STOP1   = 0x0A,  // Stop ADC1 conversions, 0000 101x (0Ah or 0Bh)
    START2  = 0x0C,  // Start ADC2 conversions, 0000 110x (0Ch or 0Dh)
    STOP2   = 0x0E,  // Stop ADC2 conversions, 0000 111x (0Eh or 0Fh)
    RDATA1  = 0x12,  // Read ADC1 data, 0001 001x (12h or 13h)
    RDATA2  = 0x14,  // Read ADC2 data, 0001 010x (14h or 15h)
    SYOCAL1 = 0x16,  // ADC1 system offset calibration, 0001 0110 (16h)
    SYGCAL1 = 0x17,  // ADC1 system gain calibration, 0001 0111 (17h)
    SFOCAL1 = 0x19,  // ADC1 self offset calibration, 0001 1001 (19h)
    SYOCAL2 = 0x1B,  // ADC2 system offset calibration, 0001 1011 (1Bh)
    SYGCAL2 = 0x1C,  // ADC2 system gain calibration, 0001 1100 (1Ch)
    SFOCAL2 = 0x1E,  // ADC2 self offset calibration, 0001 1110 (1Eh)
    RREG    = 0x20,  // Read registers 001r rrrr (20h+000r rrrr)
    RREG2   = 0x00,  // number of registers to read minus 1, 000n nnnn
    WREG    = 0x40,  // Write registers 010r rrrr (40h+000r rrrr)
    WREG2   = 0x00,  // number of registers to write minus 1, 000n nnnn
};
}  // namespace

void AdcHat::reset(void)
{
    m_ioRst.setState(IGpioPin::State::Low);
    usleep(MicrosecondsPerMillisecond * 300u);
    m_ioRst.setState(IGpioPin::State::High);
    usleep(MicrosecondsPerMillisecond * 300u);
    m_ioRst.setState(IGpioPin::State::Low);
    usleep(MicrosecondsPerMillisecond * 300);
}

void AdcHat::writeSpiCmd(uint8_t cmd)
{
    m_ioCs.setState(IGpioPin::State::High);
    m_spi.writeByte(cmd);
    m_ioCs.setState(IGpioPin::State::Low);
}

void AdcHat::writeSpiRegister(uint8_t reg, uint8_t data)
{
    m_ioCs.setState(IGpioPin::State::High);
    m_spi.writeByte(Cmd::WREG | reg);
    m_spi.writeByte(0x00);
    m_spi.writeByte(data);
    m_ioCs.setState(IGpioPin::State::Low);
}

uint8_t AdcHat::readSpiData(uint8_t reg)
{
    m_ioCs.setState(IGpioPin::State::High);
    m_spi.writeByte(Cmd::RREG | reg);
    m_spi.writeByte(0x00);
    uint8_t regValue = m_spi.readByte();
    m_ioCs.setState(IGpioPin::State::Low);
    return regValue;
}

uint8_t AdcHat::getChecksum(uint32_t val, uint8_t byte)
{
    uint8_t sum  = 0;
    uint8_t mask = -1;  // 8 bits mask, 0xff
    while (val)
    {
        sum += val & mask;  // only add the lower values
        val >>= 8;          // shift down
    }
    sum += 0x9b;
    return sum ^ byte;  // if equal, this will be 0
}

uint8_t AdcHat::readChipId(void)
{
    return readSpiData(Reg::ID);
}

bool AdcHat::config(Gain gain, DataRate drate, Delay delay)
{
    uint8_t mode2 = 0x80u;  // 0x80:PGA bypassed, 0x00:PGA enabled
    mode2 |= (gain << 4) | drate;
    writeSpiRegister(Reg::MODE2, mode2);
    usleep(MicrosecondsPerMillisecond);
    if (readSpiData(Reg::MODE2) != mode2)
    {
        LOG(error) << "AdcHat: Reg::MODE2 unsuccess";
        return false;
    }

    uint8_t refMux = 0x24u;  // 0x00:+-2.5V as REF, 0x24:VDD,VSS as REF
    writeSpiRegister(Reg::REFMUX, refMux);
    usleep(MicrosecondsPerMillisecond);
    if (readSpiData(Reg::REFMUX) != refMux)
    {
        LOG(debug) << "AdcHat: Reg::REFMUX unsuccess";
        return false;
    }

    uint8_t mode0 = delay;
    writeSpiRegister(Reg::MODE0, mode0);
    usleep(MicrosecondsPerMillisecond);
    if (readSpiData(Reg::MODE0) != mode0)
    {
        LOG(debug) << "AdcHat: Reg::MODE0 unsuccess";
        return false;
    }

    return true;
}

bool AdcHat::init(DataRate rate)
{
    reset();
    const uint8_t chipId = readChipId();
    if ((chipId >> 5u) != 1u)
    {
        LOG(error) << "AdcHat: Read chip id failed";
        return false;
    }
    LOG(debug) << "AdcHat: Chip ID: " << boost::format("0x%02x") % static_cast<uint32_t>(chipId);
    writeSpiCmd(Cmd::STOP1);
    const bool success = config(Gain::G1, rate, Delay::D35us);

    if (success)
    {
        writeSpiCmd(Cmd::START1);
    }
    return success;
}

bool AdcHat::setChannel(uint8_t channel)
{
    if (channel > 10)
    {
        LOG(error) << "AdcHat: Invalid channel number";
        return false;
    }
    uint8_t inpmux = (channel << 4) | 0x0a;  // 0x0a:VCOM as Negative Input
    writeSpiRegister(Reg::INPMUX, inpmux);
    if (readSpiData(Reg::INPMUX) != inpmux)
    {
        LOG(error) << "AdcHat: Set channel failed";
        return false;
    }
    return true;
}

uint32_t AdcHat::readData()
{
    uint32_t read   = 0;
    uint8_t  buf[4] = {0, 0, 0, 0};
    uint8_t  status, crc;
    m_ioCs.setState(IGpioPin::State::High);
    do
    {
        usleep(MicrosecondsPerMillisecond);
        m_spi.writeByte(Cmd::RDATA1);
        status = m_spi.readByte();
    } while ((status & StatusBitADC1DataReady) == 0);

    buf[0] = m_spi.readByte();
    buf[1] = m_spi.readByte();
    buf[2] = m_spi.readByte();
    buf[3] = m_spi.readByte();
    crc    = m_spi.readByte();
    m_ioCs.setState(IGpioPin::State::Low);
    read |= ((uint32_t)buf[0] << 24);
    read |= ((uint32_t)buf[1] << 16);
    read |= ((uint32_t)buf[2] << 8);
    read |= (uint32_t)buf[3];

    if (getChecksum(read, crc) != 0)
    {
        LOG(error) << "AdcHat: Data read error!";
        return 0u;
    }
    return read;
}

uint32_t AdcHat::getChannelValue(uint8_t channel)
{
    if (!setChannel(channel))
    {
        return 0u;
    }

    return readData();
}

void AdcHat::getAll(AdcHat::ValueList& values)
{
    for (uint32_t i = 0; i < MaxChannels; i++)
    {
        values[i] = getChannelValue(i);
    }
}
