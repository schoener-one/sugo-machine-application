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

#include "IHalObject.hpp"
#include "IStepperMotor.hpp"

namespace sugo::hal
{
/**
 * Class for controlling stepper motor controller.
 */
class IStepperMotorControl : public IHalObject
{
public:
    /// Stepper motor map
    using StepperMotorMap = IHalObject::Map<IStepperMotor>;

    /**
     * @brief Resets the controller and devices
     *
     */
    virtual void reset() = 0;

    /**
     * @brief Returns a map of available stepper motor devices
     *
     * @return const StepperMotorMap& A map of available stepper motor devices
     */
    virtual const StepperMotorMap& getStepperMotorMap() = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
