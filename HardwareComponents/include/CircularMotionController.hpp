/*
 * CircularMotionController.hpp
 *
 *  Created on: 30.11.2019
 *      Author: denis
 */

#ifndef CIRCULARMOTIONCONTROLLER_HPP_
#define CIRCULARMOTIONCONTROLLER_HPP_

#include <array>
#include <memory>

#include "ICircularMotionController.hpp"
#include "IPositionSwitch.hpp"
#include "IStepperMotor.hpp"

namespace moco
{
class CircularMotionController : public ICircularMotionController
{
public:
    /// Invalid position value.
    constexpr static unsigned InvalidPosition = static_cast<unsigned>(-1);

    /// Maximum of trials for finding position.
    constexpr static unsigned MaxTrials = 2u;

    /// Maximum number of position switches.
    constexpr static unsigned MaxPositionSwitches = 3u;

    using PositionSwitchArray = std::array<std::unique_ptr<IPositionSwitch>, MaxPositionSwitches>;

    CircularMotionController(IStepperMotor& stepperMotor, PositionSwitchArray& positionSwitches);

    // ICircularMotionController {{
    bool     reset() override;
    bool     rotateTo(unsigned position) override;
    unsigned getPosition() const override { return m_currentPosition; }
    bool     rotate(unsigned steps, IStepperMotor::Direction direction) override;
    // ICircularMotionController }}

private:
    IStepperMotor&       m_stepperMotor;
    PositionSwitchArray& m_positionSwitches;
    unsigned             m_currentPosition;
    unsigned             m_motorStepsToNextPosition;
};

}  // namespace moco

#endif /* CIRCULARMOTIONCONTROLLER_HPP_ */
