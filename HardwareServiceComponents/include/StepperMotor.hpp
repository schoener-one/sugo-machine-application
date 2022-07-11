///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.12.2019
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef STEPPERMOTOR_HPP_
#define STEPPERMOTOR_HPP_

#include <chrono>
#include <memory>

#include "IConfiguration.hpp"
#include "IStepperMotor.hpp"

namespace sugo
{
class StepperMotor : public IStepperMotor
{
public:
    explicit StepperMotor(const IConfiguration& configuration);
    ~StepperMotor() override;

    // IStepperMotor {{
    void     reset() override;
    unsigned runSteps(unsigned steps, Direction direction = Direction::Forward,
                      RunGuard runGuard = nullptr) override;
    void     stop() override;
    bool     runSelfTest() override;
    unsigned getStepsPerRound() const override;
    // IStepperMotor }}

private:
    unsigned m_motorIndex;
    unsigned m_motorSpeedRpm;
    bool     m_isStopped;
};

}  // namespace sugo

#endif /* STEPPERMOTOR_HPP_ */