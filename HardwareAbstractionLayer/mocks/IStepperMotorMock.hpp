//////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-08-03
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <gmock/gmock.h>

#include "IStepperMotor.hpp"

namespace sugo::hal
{
class IStepperMotorMock : public IStepperMotor
{
public:
    IStepperMotorMock() : IStepperMotor("IStepperMotorMock")
    {
    }

    MOCK_METHOD(bool, init, (const IConfiguration&));
    MOCK_METHOD(bool, reset, ());
    MOCK_METHOD(bool, rotateToPosition, (Position position));
    MOCK_METHOD(bool, rotate, (Direction direction));
    MOCK_METHOD(bool, rotate, ());
    MOCK_METHOD(bool, stop, (bool stopImmediately));
    MOCK_METHOD(Position, getPosition, (), (const));
    MOCK_METHOD(StepCount, getMicroStepCount, (), (const));
    MOCK_METHOD(StepCount, getStepsPerRound, (), (const));
    MOCK_METHOD(Speed, getSpeed, (), (const));
    MOCK_METHOD(Speed, getMaxSpeed, (), (const));
    MOCK_METHOD(void, setMaxSpeed, (Speed maxSpeed));
    MOCK_METHOD(bool, setSpeed, (Speed));
};
}  // namespace sugo::hal
