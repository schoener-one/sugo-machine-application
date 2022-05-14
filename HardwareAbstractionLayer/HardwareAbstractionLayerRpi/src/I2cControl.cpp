///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-04-28
 */
///////////////////////////////////////////////////////////////////////////////

#include "I2cControl.hpp"
#include "Logger.hpp"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>
#include <iomanip>

namespace
{
constexpr char Me[] = "I2cControl: ";
}

using namespace sugo::hal;

I2cControl::~I2cControl()
{
    if (m_fd != InvalidFileDescriptor)
    {
        finalize();
    }
}

bool I2cControl::init(const std::string& device)
{
    if (m_fd != InvalidFileDescriptor)
    {
        LOG(warning) << Me << "File descripter not closed";
        finalize();
    }

    if ((m_fd = open(device.c_str(), O_RDWR)) < 0)
    {
        LOG(error) << Me << "Failed to open device '" << device << "'";
        return false;
    }
    return true;
}

void I2cControl::finalize(void)
{
    if (m_fd != InvalidFileDescriptor)
    {
        close(m_fd);
        m_fd = InvalidFileDescriptor;
    }
}

bool I2cControl::read(Address address, const ByteBuffer& command, ByteBuffer& readBuffer)
{
    ByteBuffer tmpCommand(command);
    i2c_msg    messages[] = {
        {address, 0, static_cast<uint16_t>(tmpCommand.size()), tmpCommand.data()},
        {address, I2C_M_RD | I2C_M_STOP, static_cast<uint16_t>(readBuffer.size()), readBuffer.data()},
    };
    i2c_rdwr_ioctl_data ioctl_data = {messages, 2};

    const int retValue = ioctl(m_fd, I2C_RDWR, &ioctl_data);
    if (retValue != 2)
    {
        LOG(error) << Me << "Failed to read from device address: " << std::hex << std::setw(2)
                   << std::setfill('0') << static_cast<uint32_t>(address) << " ("
                   << std::strerror(errno) << ")";
        return false;
    }
    return true;
}

bool I2cControl::write(Address address, const ByteBuffer& command)
{
    ByteBuffer tmpCommand(command);
    i2c_msg    message = {address, 0, static_cast<uint16_t>(tmpCommand.size()), tmpCommand.data()};
    i2c_rdwr_ioctl_data ioctl_data = {&message, 1};

    const int retValue = ioctl(m_fd, I2C_RDWR, &ioctl_data);
    if (retValue != 1)
    {
        LOG(error) << Me << "Failed to write to device address: " << std::hex << std::setw(2)
                   << std::setfill('0') << static_cast<uint32_t>(address) << " ("
                   << std::strerror(errno) << ")";
        return false;
    }
    return true;
}
