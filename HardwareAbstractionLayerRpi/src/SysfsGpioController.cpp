/*
 * SysfsGpioController.cpp
 *
 *  Created on: 15.12.2019
 *      Author: denis
 */
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cassert>
#include <fstream>

#include "Globals.hpp"
#include "SysfsGpioController.hpp"

using namespace sugo;

const std::string SysfsGpioController::GpioBasePath("/sys/class/gpio");

inline bool WriteFile(const std::string& fileName, const std::string& value)
{
    bool          success = true;
    std::ofstream file(fileName);
    if (file)
    {
        file << value;
        if (file.fail())
        {
            LOG(error) << "Failed to write to file '" << fileName << "' (" << strerror(errno)
                       << ")";
            success = false;
        }
        file.close();
    }
    else
    {
        LOG(error) << "Failed to open file '" << fileName << "' (" << strerror(errno) << ")";
        success = false;
    }
    return success;
}

inline bool ReadFile(const std::string& fileName, std::string& value)
{
    bool          success = true;
    std::ifstream file(fileName);
    if (file)
    {
        if (!std::getline(file, value))
        {
            LOG(error) << "Failed to write to file '" << fileName << "' (" << strerror(errno)
                       << ")";
            success = false;
        }
        file.close();
    }
    else
    {
        LOG(error) << "Failed to open file '" << fileName << "' (" << strerror(errno) << ")";
        success = false;
    }
    return success;
}

bool SysfsGpioController::registerPin(unsigned pin, PinDirection pinDirection)
{
    assert(pin < MaxPinCount);
    assert(m_pinInfoList[pin].file == InvalidPin);
    const std::string direction = (pinDirection == Out ? "out" : "in");

    LOG(debug) << "Register GPIO " << direction << " pin " << pin;
    m_pinInfoList[pin].path = GpioBasePath + "/gpio" + std::to_string(pin);

    std::string setDirection;
    bool        success = ReadFile(m_pinInfoList[pin].path + "/direction", setDirection);

    if (success && (setDirection != direction))
    {
        LOG(error) << "Pin direction wrong configured - direction '" << direction
                   << "' requested but '" << setDirection << "' found";
        success = false;
    }

    if (success)
    {
        if (pinDirection == In)
        {
            LOG(debug) << "Preparing GPIO input pin " << pin;
            success = prepareInPin(pin);
        }
        else
        {
            LOG(debug) << "Preparing GPIO output pin " << pin;
            success = prepareOutPin(pin);
        }
    }

    return success;
}

bool SysfsGpioController::prepareOutPin(unsigned pin)
{
    bool              success   = true;
    const std::string valuePath = m_pinInfoList[pin].path + "/value";

    if ((m_pinInfoList[pin].file = ::open(valuePath.c_str(), O_WRONLY | O_NONBLOCK)) < 0)
    {
        LOG(error) << "Failed to open GPIO value file '" << valuePath << "' (" << strerror(errno)
                   << ")";
        success                 = false;
        m_pinInfoList[pin].file = InvalidPin;
    }

    if (success)
    {
        LOG(debug) << "Setting initial GPIO state to 'Low' for pin " << pin;
        success = setPinState(pin, PinState::Low);
    }
    return success;
}

bool SysfsGpioController::prepareInPin(unsigned pin)
{
    // Set fileDescriptor edge detection
    LOG(debug) << "Writing GPIO edge file for pin " << pin;
    bool success = WriteFile(m_pinInfoList[pin].path + "/edge", "both");

    const std::string valuePath = m_pinInfoList[pin].path + "/value";

    if ((m_pinInfoList[pin].file = ::open(valuePath.c_str(), O_RDONLY | O_NONBLOCK)) < 0)
    {
        LOG(error) << "Failed to open GPIO value file '" << valuePath << "' (" << ::strerror(errno)
                   << ")";
        success                 = false;
        m_pinInfoList[pin].file = InvalidPin;
    }

    if (success)
    {
        LOG(debug) << "Reading initial GPIO state from pin " << pin;
        PinState state;
        success = getPinState(pin, state);
    }

    return success;
}

// cppcheck-suppress unusedFunction
bool SysfsGpioController::unregisterPin(unsigned pin)
{
    assert(m_pinInfoList[pin].file != InvalidPin);
    const std::string direction = (m_pinInfoList[pin].direction == Out ? "out" : "in");

    LOG(debug) << "Unregister GPIO " << direction << " pin " << pin;

    (void)::close(m_pinInfoList[pin].file);
    LOG(debug) << "Writing GPIO export file for pin " << pin;
    const bool success = WriteFile(GpioBasePath + "/unexport", std::to_string(pin));
    m_pinInfoList[pin].clear();
    return success;
}

bool SysfsGpioController::getPinState(unsigned pin, IGpioController::PinState& outPinState) const
{
    assert(m_pinInfoList[pin].file != InvalidPin);
    char value = 0;

    (void)::lseek(m_pinInfoList[pin].file, 0, SEEK_SET);
    const ssize_t byteCount = ::read(m_pinInfoList[pin].file, &value, sizeof(value));
    bool          success   = (byteCount == sizeof(value));

    if (success)
    {
        const bool isLow = ((value - '0') == 0) ? m_isActiveHigh : !m_isActiveHigh;
        outPinState      = isLow ? PinState::Low : PinState::High;
    }

    return success;
}

bool SysfsGpioController::setPinState(unsigned pin, PinState pinState)
{
    assert(m_pinInfoList[pin].file != InvalidPin);
    const char    value     = (pinState == PinState::Low) ? '0' : '1';
    const ssize_t byteCount = ::write(m_pinInfoList[pin].file, &value, sizeof(value));
    return (byteCount == sizeof(value));
}

bool SysfsGpioController::waitForPinStateChange(unsigned                   pin,
                                                IGpioController::PinState& outPinState,
                                                int                        timeoutMS) const
{
    struct pollfd pollInfo;
    pollInfo.fd       = m_pinInfoList[pin].file;
    pollInfo.events   = POLLPRI | POLLERR;
    pollInfo.revents  = 0;
    const int rv      = ::poll(&pollInfo, 1, timeoutMS);
    bool      success = false;
    if (rv > 0)
    {
        // cppcheck-suppress comparisonError
        success = (pollInfo.revents & POLLPRI) > 0;
    }
    else if (rv < 0)
    {
        LOG(error) << "Failed to wait state change of pin " << pin << " (" << ::strerror(errno)
                   << ")";
    }

    if (success)
    {
        success = getPinState(pin, outPinState);
    }
    return success;
}
