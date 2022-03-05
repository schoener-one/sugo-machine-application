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

#include "HalObject.hpp"
#include "IConfiguration.hpp"
#include "IStepperMotorControl.hpp"

namespace sugo::hal
{
/**
 * @brief Class for contolling stepper motors
 *
 */
class StepperMotorControl : public HalObject, public IStepperMotorControl
{
public:
    /**
     * @brief Construct a new stepper motor control object
     *
     * @param id Identifier of that object
     */
    StepperMotorControl(const Identifier& id) : HalObject(id) {}

    /**
     * @brief Initializes the object.
     *
     * @param configuration Configuration to be applied for initialization
     * @return true  If the object could be initialized successfully
     * @return false If the object could not be initialized
     */
    bool init(const IConfiguration& configuration);

    void reset() override;

    const StepperMotorMap& getStepperMotorMap() override { return m_stepperMotorMap; }

private:
    StepperMotorMap m_stepperMotorMap;
};

}  // namespace sugo::hal
