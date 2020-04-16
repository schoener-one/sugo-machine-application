/*
 * PositionSwitch.cpp
 *
 *  Created on: 05.12.2019
 *      Author: denis
 */

#include <cassert>

#include "IGpioController.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "PositionSwitch.hpp"

using namespace moco;

PositionSwitch::PositionSwitch(unsigned switchGpioPin)
    : m_switchGpioPin(switchGpioPin)
{
    IHardwareAbstractionLayer& hal = IHardwareAbstractionLayer::get();
    const bool success = hal.getGpioController().registerPin(m_switchGpioPin, IGpioController::In);
    // FIXME add proper error handling
    assert(success == true);
}

bool PositionSwitch::isOpen() const
{
    IHardwareAbstractionLayer& hal = IHardwareAbstractionLayer::get();
    IGpioController::PinState  pinState;
    const bool success = hal.getGpioController().getPinState(m_switchGpioPin, pinState);
    // FIXME add proper error handling
    assert(success == true);
    return (pinState == IGpioController::Low);
}

// cppcheck-suppress unusedFunction
bool PositionSwitch::waitForChange(int timeoutMS) const
{
    IHardwareAbstractionLayer& hal     = IHardwareAbstractionLayer::get();
    const bool                 wasOpen = isOpen();
    IGpioController::PinState  pinState;
    (void)hal.getGpioController().waitForPinStateChange(m_switchGpioPin, pinState, timeoutMS);
    return wasOpen != isOpen();
}
