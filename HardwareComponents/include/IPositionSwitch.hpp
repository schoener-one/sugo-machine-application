/*
 * IPositionSwitchObserver.hpp
 *
 *  Created on: 30.11.2019
 *      Author: denis
 */

#ifndef IPOSITIONSWITCH_HPP_
#define IPOSITIONSWITCH_HPP_

namespace moco
{
class IPositionSwitch
{
protected:
    IPositionSwitch() {}

public:
    virtual ~IPositionSwitch() {}

    virtual bool isOpen() const = 0;
    /**
     * Waits until position switch state change.
     *
     * @param timeoutMS Maximum time to wait for a change. If timeoutMS is set
     *                  to -1, the caller is blocked forever until a change
     *                  occurs.
     * @return Returns true if the position switch state has changed or false
     *         if a timeout has occurred.
     */
    virtual bool waitForChange(int timeoutMS) const = 0;
};

}  // namespace moco

#endif /* IPOSITIONSWITCH_HPP_ */
