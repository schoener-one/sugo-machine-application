/*
 * PositionSwitchObserver.hpp
 *
 *  Created on: 01.12.2019
 *      Author: denis
 */

#ifndef POSITIONSWITCHOBSERVER_HPP_
#define POSITIONSWITCHOBSERVER_HPP_

#include <memory>

#include "IHardwareAbstractionLayer.hpp"
#include "IPositionSwitch.hpp"

namespace moco
{
class PositionSwitch : public IPositionSwitch
{
public:
    explicit PositionSwitch(unsigned switchGpioPin);

    // IPositionSwitch {{
    bool isOpen() const override;
    bool waitForChange(int timeoutMS) const override;
    // IPositionSwitch }}

private:
    const unsigned m_switchGpioPin;
};

}  // namespace moco

#endif /* POSITIONSWITCHOBSERVER_HPP_ */
