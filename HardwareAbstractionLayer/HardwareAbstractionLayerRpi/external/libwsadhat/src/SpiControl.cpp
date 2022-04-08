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

#include "SpiControl.hpp"
#include "Logger.hpp"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace wsadhat;

SpiControl::~SpiControl()
{
    if (m_fd != InvalidFileDescriptor)
    {
        finalize();
    }
}

bool SpiControl::init(const std::string& device)
{
    // device
    if ((m_fd = open(device.c_str(), O_RDWR)) < 0)
    {
        LOG(error) << "SpiControl: Failed to open SPI device";
        return false;
    }
    m_mode = 0;
    memset(&m_tr, 0, sizeof(m_tr));

    uint8_t bits = 8;
    if (ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1)
    {
        LOG(error) << "SpiControl: can't set bits per word";
        finalize();
        return false;
    }

    if (ioctl(m_fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1)
    {
        LOG(error) << "SpiControl: can't get bits per word";
        finalize();
        return false;
    }
    m_tr.bits_per_word = bits;
    m_tr.delay_usecs   = 0u;

    bool success = setMode(SPIMode::Mode1);

    if (success)
    {
        success = setChipSelect(SPIChipSelect::Low);
    }

    if (success)
    {
        success = setBitOrder(SPIBitOrder::MsbFirst);
    }

    if (success)
    {
        success = setSpeed(1000000u);
    }

    if (!success)
    {
        finalize();
    }

    return success;
}

void SpiControl::finalize(void)
{
    m_mode = 0;
    if (m_fd != InvalidFileDescriptor)
    {
        if (close(m_fd) != 0)
        {
            LOG(error) << "SpiControl: Failed to close SPI device";
        }
        m_fd = InvalidFileDescriptor;
    }
}

bool SpiControl::setSpeed(uint32_t speed)
{
    // Write speed
    if (ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
    {
        LOG(debug) << "SpiControl: can't set max speed hz";
        return false;
    }

    // Read the speed of just writing
    if (ioctl(m_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1)
    {
        LOG(debug) << "SpiControl: can't get max speed hz";
        return false;
    }
    m_tr.speed_hz = speed;
    return true;
}

bool SpiControl::setMode(SPIMode mode)
{
    m_mode &= 0xfC;  // Clear low 2 digits
    switch (mode)
    {
        case Mode0:
            m_mode |= SPI_MODE_0;
            break;
        case Mode1:
            m_mode |= SPI_MODE_1;
            break;
        case Mode2:
            m_mode |= SPI_MODE_2;
            break;
        case Mode3:
            m_mode |= SPI_MODE_3;
            break;
    }

    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1)
    {
        LOG(error) << "SpiControl: can't set spi mode";
        return false;
    }
    return true;
}

bool SpiControl::setChipSelect(SPIChipSelect mode)
{
    switch (mode)
    {
        case SPIChipSelect::None:
            m_mode |= SPI_NO_CS;
            break;
        case SPIChipSelect::Low:
            m_mode &= ~SPI_CS_HIGH;
            m_mode &= ~SPI_NO_CS;
            break;
        case SPIChipSelect::High:
            m_mode |= SPI_CS_HIGH;
            m_mode &= ~SPI_NO_CS;
            break;
    }

    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1)
    {
        LOG(error) << "SpiControl: can't set spi mode";
        return false;
    }
    return true;
}

int SpiControl::setBitOrder(SPIBitOrder order)
{
    switch (order)
    {
        case SPIBitOrder::LsbFirst:
            m_mode |= SPI_LSB_FIRST;
            break;
        case SPIBitOrder::MsbFirst:
            m_mode &= ~SPI_LSB_FIRST;
            break;
    }

    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1)
    {
        LOG(error) << "SpiControl: can't set spi SPI_LSB_FIRST";
        return false;
    }
    return true;
}

int SpiControl::setBusMode(BusMode mode)
{
    switch (mode)
    {
        case BusMode::Mode3Wire:
            m_mode |= SPI_3WIRE;
            break;
        case BusMode::Mode4Wire:
            m_mode &= ~SPI_3WIRE;
            break;
    }

    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1)
    {
        LOG(error) << "SpiControl: can't set spi mode";
        return false;
    }
    return true;
}

uint8_t SpiControl::writeByte(uint8_t buf)
{
    uint8_t rbuf[1];
    m_tr.len    = 1;
    m_tr.tx_buf = (unsigned long)&buf;
    m_tr.rx_buf = (unsigned long)rbuf;

    if (ioctl(m_fd, SPI_IOC_MESSAGE(1), &m_tr) < 1)
    {
        LOG(error) << "SpiControl: can't send spi message";
    }
    return rbuf[0];
}
