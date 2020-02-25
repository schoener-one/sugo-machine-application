/*
 * StepperMotor.hpp
 *
 *  Created on: 01.12.2019
 *      Author: denis
 */

#ifndef STEPPERMOTOR_HPP_
#define STEPPERMOTOR_HPP_

#include <memory>
#include <chrono>

#include "IStepperMotor.hpp"

namespace moco
{

class StepperMotor : public IStepperMotor
{
public:
	static constexpr unsigned DefaultSpeedRpm = 10u;
	static constexpr unsigned DefaultMotorIndex = 0u;

	StepperMotor(unsigned motorSpeedRpm = DefaultSpeedRpm,
			unsigned motorIndex = DefaultMotorIndex);
	~StepperMotor() override;

	// IStepperMotor {{
	void reset() override;
	unsigned runSteps(unsigned steps, Direction direction = Direction::Forward,
			RunGuard runGuard = nullptr) override;
	void stop() override;
	bool runSelfTest() override;
	unsigned getStepsPerRound() const override;
	// IStepperMotor }}

private:
	unsigned m_motorIndex;
	unsigned m_motorSpeedRpm;
	bool m_isStopped;
};

} // namespace moco

#endif /* STEPPERMOTOR_HPP_ */
