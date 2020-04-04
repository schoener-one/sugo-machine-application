/*
 * SysfsGpioController.hpp
 *
 *  Created on: 15.12.2019
 *      Author: denis
 */

#ifndef SYSFSGPIOCONTROLLER_HPP_
#define SYSFSGPIOCONTROLLER_HPP_

#include <string>

#include "IGpioController.hpp"

namespace moco
{
/**
 * Class implementing GPIO control on sysfs GPIO interface.
 */
class SysfsGpioController : public IGpioController
{
public:
    static const std::string GpioBasePath;

    explicit SysfsGpioController(bool isActiveHigh) : m_isActiveHigh(isActiveHigh) {}

    // IHardwareAbstractionLayer::IStepperMotorController::IGpioController {{
    bool registerPin(unsigned pin, PinDirection pinDirection) override;
    bool unregisterPin(unsigned pin) override;
    bool getPinState(unsigned pin, PinState& outPinState) const override;
    bool setPinState(unsigned pin, PinState pinState) override;
    bool waitForPinStateChange(unsigned pin, PinState& outPinState, int timeoutMS) const override;
    // IHardwareAbstractionLayer::IStepperMotorController::IGpioController }}

private:
    constexpr static int      InvalidPin  = UINT32_MAX;
    constexpr static unsigned MaxPinCount = 16;

    struct PinInfo
    {
        PinInfo() : file(InvalidPin), direction(In) {}

        void clear()
        {
            path.clear();
            file = InvalidPin;
        }

        std::string  path;
        int          file;
        PinDirection direction;
    };

    bool prepareInPin(unsigned pin);
    bool prepareOutPin(unsigned pin);

    const bool m_isActiveHigh;
    PinInfo    m_pinInfoList[MaxPinCount];
};

}  // namespace moco

#endif /* SYSFSGPIOCONTROLLER_HPP_ */
