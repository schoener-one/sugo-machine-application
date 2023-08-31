///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <ostream>

#include "HardwareAbstractionLayer/IHalObject.hpp"
#include "HardwareAbstractionLayer/UnitValue.hpp"

namespace sugo::hal
{
/// @brief Class representing one stepper motor control.
class IStepperMotor : public IHalObject
{
public:
    using StepCount = unsigned;
    /// Speed value type
    using Speed = UnitValue<unsigned>;
    /// Position type
    using Position = int32_t;

    /// Direction of rotation.
    enum Direction
    {
        Forward,  ///< forward
        Backward  ///< backward
    };

    /**
     * @brief Destroy the IStepperMotor object.
     *
     */
    ~IStepperMotor() override = default;

    /**
     * @brief Resets the device to the default state.
     * If it was running before it will be stopped. All parameters like
     * speed will also be set to default.
     *
     * @return true  if the reset was successful.
     * @return false if the reset failed.
     */
    virtual bool reset() = 0;

    /**
     * @brief Rotates the stepper motor to passed position. The method blocks the caller
     *        until the end of rotation is reached.
     *
     * @param position Position which should be reached.
     * @param direction Direction of rotation.
     * @return true If the position could be reached.
     * @return false If the position could not be reached.
     * @note Before the rotation can be done, the speed has to be properly set!
     */
    virtual bool rotateToPosition(Position position) = 0;

    /**
     * @brief Starts rotating the stepper motor continuously.
     * The caller will not be blocked by that call. The rotation has to be
     * stopped by a stop() call.
     *
     * @param direction Direction of rotation.
     * @return true If the motor could be started.
     * @return false If the motor could not be started.
     */
    virtual bool rotate(Direction direction) = 0;

    /**
     * @brief Starts rotating the stepper motor continuously.
     * The caller will not be blocked by that call. The rotation has to be
     * stopped by a stop() call. The
     *
     * @note The configured rotation direction is used.
     * @return true If the motor could be started.
     * @return false If the motor could not be started.
     */
    virtual bool rotate() = 0;

    /**
     * @brief Stops a running rotation. The method blocks the caller
     *        until the motor has stopped rotation.
     *
     * @param stopImmediately Indicates if the motor should be stopped immediately (true) or
     *                        by decreasing speed (false).
     * @return true If the motor was rotating and has been stopped
     * @return false If the motor was not rotating
     */
    virtual bool stop(bool stopImmediately) = 0;

    /**
     * @brief Returns the current position of the motor.
     *
     * @return Position Current position of the motor.
     */
    virtual Position getPosition() const = 0;

    /**
     * @brief Returns the micro steps per step for the motor.
     *
     * @return unsigned Number of micro steps per step
     */
    virtual StepCount getMicroStepCount() const = 0;

    /**
     * @brief Returns the complete number of steps to be used for a 360° turn.
     * If micro steps are used, that value returns the micro step count per round.
     *
     * @return unsigned Number of steps per round
     */
    virtual StepCount getStepsPerRound() const = 0;

    /**
     * @brief Returns the current motor speed.
     *
     * @return Speed The current motor speed.
     */
    virtual Speed getSpeed() const = 0;

    /**
     * @brief Returns the max motor speed
     *
     * @return unsigned Maximum motor speed in RPM.
     */
    virtual Speed getMaxSpeed() const = 0;

    /**
     * @brief Set the max motor speed.
     *
     * @param maxSpeed Sets the max speed in RPMs.
     */
    virtual void setMaxSpeed(Speed maxSpeed) = 0;

    /**
     * @brief Sets the target speed of the motor.
     *
     * @note The rotation direction cannot be changed until motor has been stopped again!
     * @param speed Velocity to be set.
     * @return true If the speed could be set successfully.
     * @return false If the speed could not be set successfully.
     */
    virtual bool setSpeed(Speed speed) = 0;

    /**
     * @brief Returns the time between one step to the next step position.
     *
     * @param rpm Speed of the rotation.
     * @return std::chrono::microseconds The time between one step to the next step position.
     */
    inline std::chrono::microseconds getTimePerStep(unsigned rpm) const
    {
        using namespace std::literals::chrono_literals;
        return (60000000us / static_cast<int64_t>(rpm * getStepsPerRound()));
    }

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal

// Helper functions for type to string convertion
namespace std
{
/**
 * @brief std::ostream type presentation helper function
 */
inline std::ostream& operator<<(std::ostream& ostr, sugo::hal::IStepperMotor::Direction const& dir)
{
    ostr << (dir == sugo::hal::IStepperMotor::Direction::Forward ? "Forward" : "Backward");
    return ostr;
}
}  // namespace std
