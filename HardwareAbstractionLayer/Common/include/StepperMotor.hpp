///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IStepperMotor.hpp"

namespace sugo::hal
{
/**
 * @brief Class represents an ADC input
 *
 */
class StepperMotor : public IStepperMotor
{
public:
    using IStepperMotor::IStepperMotor;

    bool init(const IConfiguration& configuration) override;

    unsigned rotate(unsigned steps, Direction direction, unsigned speed) override;
    bool     stop() override;
    unsigned getMicroStepCount() const override;
    unsigned getStepsPerRound() const override;
    unsigned getSpeed() const override;
    unsigned getMaxSpeed() const override;

private:
    unsigned m_i2cId    = InvalidI2cId;
    unsigned m_maxSpeed = 0;
};

}  // namespace sugo::hal
