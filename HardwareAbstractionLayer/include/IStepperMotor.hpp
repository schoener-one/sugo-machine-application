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
class IStepperMotor : virtual public IHalObject
{
public:
    enum Direction
    {
        Forward,
        Backward
    };

protected:
    IStepperMotor()
    {
    }

public:
    virtual ~IStepperMotor()
    {
    }

    virtual void              step(Direction direction) = 0;
    virtual unsigned          getMicroStepCount() const = 0;
    virtual unsigned          getStepsPerRound() const  = 0;
    std::chrono::microseconds getTimePerStep(unsigned rpm) const
    {
        using namespace std::literals::chrono_literals;
        return (60000000us / static_cast<int64_t>(rpm * getStepsPerRound()));
    }
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
