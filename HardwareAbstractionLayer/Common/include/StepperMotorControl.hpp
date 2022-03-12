///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IConfiguration.hpp"
#include "IStepperMotorControl.hpp"

namespace sugo::hal
{
/**
 * @brief Class for contolling stepper motors
 *
 */
class StepperMotorControl : public IStepperMotorControl
{
public:
    using IStepperMotorControl::IStepperMotorControl;

    bool init(const IConfiguration& configuration) override;

    void reset() override;

    const StepperMotorMap& getStepperMotorMap() override
    {
        return m_stepperMotorMap;
    }

private:
    StepperMotorMap m_stepperMotorMap;
};

}  // namespace sugo::hal
