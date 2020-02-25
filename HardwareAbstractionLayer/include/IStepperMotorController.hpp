/*
 * IStepperMotorController.hpp
 *
 *  Created on: 15.12.2019
 *      Author: denis
 */

#ifndef ISTEPPERMOTORCONTROLLER_HPP_
#define ISTEPPERMOTORCONTROLLER_HPP_

#include <chrono>

namespace moco
{
/**
 * Class for controlling stepper motor controller.
 */
class IStepperMotorController
{
public:
    /**
     * Class for controlling stepper motor.
     */
    class IMotorControl
    {
    public:
        enum Direction
        {
            Forward,
            Backward
        };

    protected:
        IMotorControl() {}

    public:
        virtual ~IMotorControl() {}

        virtual void              step(Direction direction) = 0;
        virtual unsigned          getMicroStepCount() const = 0;
        virtual unsigned          getStepsPerRound() const  = 0;
        std::chrono::microseconds getTimePerStep(unsigned rpm) const
        {
            using namespace std::literals::chrono_literals;
            return (60000000us / static_cast<int64_t>(rpm * getStepsPerRound()));
        }
    };

protected:
    IStepperMotorController() {}

public:
    virtual ~IStepperMotorController() {}

    virtual void           reset()                          = 0;
    virtual unsigned       getStepperMotorCount() const     = 0;
    virtual IMotorControl& getMotorControl(unsigned number) = 0;
};

}  // namespace moco

#endif /* ISTEPPERMOTORCONTROLLER_HPP_ */
