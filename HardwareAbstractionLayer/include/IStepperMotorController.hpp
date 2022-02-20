///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2019-12-15
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

namespace sugo
{
/**
 * Class for controlling stepper motor controller.
 */
class IStepperMotorController
{
public:
    /**
     * Class representing one stepper motor control.
     */
    class IStepperMotorControl
    {
    public:
        enum Direction
        {
            Forward,
            Backward
        };

    protected:
        IStepperMotorControl() {}

    public:
        virtual ~IStepperMotorControl() {}

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

    virtual void                  reset()                                 = 0;
    virtual unsigned              getStepperMotorCount() const            = 0;
    virtual IStepperMotorControl& getStepperMotorControl(unsigned number) = 0;
};

}  // namespace sugo

#endif /* ISTEPPERMOTORCONTROLLER_HPP_ */
