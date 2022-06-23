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
#include "IGpioPin.hpp"

namespace sugo::hal
{
class I2cControl;

/**
 * @brief Class for contolling stepper motors
 *
 */
class StepperMotorControl : public IStepperMotorControl
{
public:
    StepperMotorControl(const Identifier& id, IGpioPin& ioErr, IGpioPin& ioRst)
        : IStepperMotorControl(id), m_ioErr(ioErr), m_ioRst(ioRst)
    {
    }
    ~StepperMotorControl() override;
    
    bool init(const IConfiguration& configuration) override;

    void finalize();

    void reset() override;

    const StepperMotorMap& getStepperMotorMap() override
    {
        return m_stepperMotorMap;
    }

private:
    IGpioPin&       m_ioErr;
    IGpioPin&       m_ioRst;
    I2cControl*     m_i2c = nullptr;
    StepperMotorMap m_stepperMotorMap;
};

}  // namespace sugo::hal
