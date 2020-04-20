/*
 * IGpioController.hpp
 *
 *  Created on: 15.12.2019
 *      Author: denis
 */

#ifndef IGPIOCONTROLLER_HPP_
#define IGPIOCONTROLLER_HPP_

namespace moco
{
/**
 * Class for contolling GPIOs.
 */
class IGpioController
{
public:
    enum PinState
    {
        Low = 0,
        High
    };
    enum PinDirection
    {
        In,
        Out
    };

    /// Infinite wait time for waitForPinStateChange.
    constexpr static int InfiniteTimeout = -1;

protected:
    IGpioController() {}

public:
    virtual ~IGpioController() {}

    virtual bool registerPin(unsigned pin, PinDirection pinDirection)   = 0;
    virtual bool unregisterPin(unsigned pin)                            = 0;
    virtual bool getPinState(unsigned pin, PinState& outPinState) const = 0;
    virtual bool setPinState(unsigned pin, PinState pinState)           = 0;
    virtual bool waitForPinStateChange(unsigned pin, PinState& outPinState,
                                       int timeoutMS) const             = 0;
};

}  // namespace moco

#endif /* IGPIOCONTROLLER_HPP_ */
