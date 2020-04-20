/*
 * CircularMotionController.cpp
 *
 *  Created on: 30.11.2019
 *      Author: denis
 */

#include <cmath>

#include "CircularMotionController.hpp"
#include "Globals.hpp"

using namespace moco;

CircularMotionController::CircularMotionController(IStepperMotor&       stepperMotor,
                                                   PositionSwitchArray& positionSwitches)
    : m_stepperMotor(stepperMotor),
      m_positionSwitches(positionSwitches),
      m_currentPosition(InvalidPosition),
      m_motorStepsToNextPosition(stepperMotor.getStepsPerRound() + 2 / 3)
{
}

bool CircularMotionController::reset()
{
    m_stepperMotor.reset();
    m_currentPosition = 0;
    return true;
}

bool CircularMotionController::rotateTo(unsigned positionSwitchIndex)
{
    assert(positionSwitchIndex < MaxPositionSwitches);
    bool success = false;

    if (m_currentPosition != InvalidPosition)
    {
        LOG(debug) << "Start rotating to position switch " << positionSwitchIndex << " with "
                   << m_motorStepsToNextPosition << " steps";
        const auto direction = (positionSwitchIndex > m_currentPosition)
                                   ? IStepperMotor::Direction::Forward
                                   : IStepperMotor::Direction::Backward;
        const unsigned stepsNeeded = static_cast<unsigned>(
            std::abs(static_cast<int>(positionSwitchIndex) - static_cast<int>(m_currentPosition)));
        unsigned stepsUsed = 0;
        // Run the steps until position switch is closed or the max overruns steps are exhausted!
        for (unsigned i = 0;
             (i < (MaxTrials + stepsNeeded)) && m_positionSwitches[positionSwitchIndex]->isOpen();
             i++)
        {
            assert(m_motorStepsToNextPosition > 0);
            stepsUsed += m_stepperMotor.runSteps(m_motorStepsToNextPosition, direction, [&] {
                return m_positionSwitches[positionSwitchIndex]->isOpen();
            });
        }
        m_stepperMotor.reset();  // turn motor control off again!
        success = !m_positionSwitches[positionSwitchIndex]->isOpen();
        LOG(debug) << "Rotation stopped with " << ((success) ? "success" : "failure") << " after "
                   << stepsUsed << " steps";

        if (success)
        {
            // Adapt needed positions
            // TODO add a controlling here
            const unsigned onePercentOfSteps = m_motorStepsToNextPosition / 100u;
            if (stepsUsed > m_motorStepsToNextPosition)
            {
                m_motorStepsToNextPosition += onePercentOfSteps;
            }
            else if (stepsUsed < m_motorStepsToNextPosition)
            {
                m_motorStepsToNextPosition -= onePercentOfSteps;
            }
        }
    }
    else
    {
        LOG(error) << "Motor not initialized";
    }
    return success;
}

bool CircularMotionController::rotate(unsigned steps, IStepperMotor::Direction direction)
{
    const unsigned stepsDone = m_stepperMotor.runSteps(steps, direction);
    m_stepperMotor.reset();  // turn motor control off again!
    return (stepsDone == steps);
}
