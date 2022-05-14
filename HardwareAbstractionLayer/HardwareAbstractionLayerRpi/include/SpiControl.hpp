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

#include <linux/spi/spidev.h>
#include <stdint.h>
#include <string>

namespace sugo::hal
{
/**
 * @brief Control class for SPI interface
 *
 */
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

    struct spi_ioc_transfer m_tr;
    int                     m_fd = InvalidFileDescriptor;
    uint16_t                m_mode;
};

}  // namespace sugo::hal
