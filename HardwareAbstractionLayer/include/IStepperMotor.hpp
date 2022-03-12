///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-04-07
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>
#include <ostream>

#include "IHalObject.hpp"

namespace sugo::hal
{
/**
 * Class representing one stepper motor control.
 */
class IStepperMotor : public IHalObject
{
public:
    /// Invalid I2C identifier.
    constexpr static unsigned InvalidI2cId = 0u;

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
    virtual ~IStepperMotor()
    {
    }

    /**
     * @brief Rotates the stepper motor. The method blocks the caller
     *        until the end of rotation is reached.
     *
     * @param steps Number of steps to rotate.
     * @param direction Direction of rotation.
     * @param speed Speed of rotation in RPM. The speed cannot be higher then the maximum!
     * @return unsigned The number of steps rotated. If the returned value does not match the
     * passed, the rotation has been stopped earlier!
     */
    virtual unsigned rotate(unsigned steps, Direction direction, unsigned speed) = 0;

    /**
     * @brief Stops a running rotation. The method blocks the caller
     *        until the motor has stopped rotation.
     *
     * @return true If the motor was rotating and has been stopped
     * @return false If the motor was not rotating
     */
    virtual bool stop() = 0;

    virtual unsigned getMicroStepCount() const = 0;
    virtual unsigned getStepsPerRound() const  = 0;
    virtual unsigned getSpeed() const          = 0;
    virtual unsigned getMaxSpeed() const       = 0;

    /**
     * @brief Returns the time between one step to the next step position.
     *
     * @param rpm Speed of the rotation.
     * @return std::chrono::microseconds The time between one step to the next step position.
     */
    std::chrono::microseconds getTimePerStep(unsigned rpm) const
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
