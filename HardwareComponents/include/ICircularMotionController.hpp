/*
 * ICirularMotionController.hpp
 *
 *  Created on: 30.11.2019
 *      Author: denis
 */

#ifndef ICIRCULARMOTIONCONTROLLER_HPP_
#define ICIRCULARMOTIONCONTROLLER_HPP_

#include "IMotionController.hpp"
#include "IStepperMotor.hpp"

namespace moco
{
class ICircularMotionController : public IMotionController
{
protected:
    ICircularMotionController() {}

public:
    virtual ~ICircularMotionController() {}

    virtual bool     reset()                                                    = 0;
    virtual bool     rotateTo(unsigned position)                                = 0;
    virtual unsigned getPosition() const                                        = 0;
    virtual bool     rotate(unsigned steps, IStepperMotor::Direction direction) = 0;
};

}  // namespace moco

#endif /* ICIRCULARMOTIONCONTROLLER_HPP_ */
