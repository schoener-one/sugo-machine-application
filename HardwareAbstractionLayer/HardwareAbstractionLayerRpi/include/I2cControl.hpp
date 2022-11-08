
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-04-28
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "HalTypes.hpp"

#include <string>

namespace sugo::hal
{
/**
 * @brief Control class for handling I2C messaging.
 *
 */
class I2cControl
{
public:
    using Address = uint8_t;

    I2cControl() = default;
    ~I2cControl();

    bool init(const std::string& device);
    void finalize(void);

    bool read(Address address, const ByteBuffer& command, ByteBuffer& retValue) const;
    bool write(Address address, const ByteBuffer& command) const;

private:
    constexpr static int InvalidFileDescriptor = -1;

    int m_fd = InvalidFileDescriptor;
};
}  // namespace sugo::hal