///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#include "StepperMotorControl.hpp"
#include "HalHelper.hpp"
#include "StepperMotor.hpp"

using namespace sugo::hal;

void StepperMotorControl::reset()
{
}

bool StepperMotorControl::init(const IConfiguration& configuration)
{
    return initEnabledSubComponents<IStepperMotor, StepperMotor>(configuration, "motor",
                                                                 m_stepperMotorMap);
}
