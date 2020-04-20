/*
 * IStepperMotor.hpp
 *
 *  Created on: 30.11.2019
 *      Author: denis
 */

#ifndef ISTEPPERMOTOR_HPP_
#define ISTEPPERMOTOR_HPP_

#include <functional>

namespace moco
{
class IStepperMotor
{
protected:
    IStepperMotor() {}

public:
    /**
     * Direction of the motor
     */
    enum Direction
    {
        Forward,  //!< Forward
        Backward  //!< Backward
    };

    /**
     * Guard function which checks if the stepper motor should keep running.
     */
    using RunGuard = std::function<bool(void)>;

    virtual ~IStepperMotor() {}

    virtual void     reset()                                             = 0;
    virtual unsigned runSteps(unsigned steps, Direction direction,
                              RunGuard runGuard = []() { return true; }) = 0;
    virtual void     stop()                                              = 0;
    virtual bool     runSelfTest()                                       = 0;
    virtual unsigned getStepsPerRound() const                            = 0;
};

}  // namespace moco

#endif /* ISTEPPERMOTOR_HPP_ */
