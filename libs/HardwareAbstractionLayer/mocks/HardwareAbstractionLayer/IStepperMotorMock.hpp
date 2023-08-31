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

#include <gmock/gmock.h>

#include "HardwareAbstractionLayer/IStepperMotor.hpp"

namespace sugo::hal
{
/// @brief Mock class of IStepperMotor
class IStepperMotorMock : public IStepperMotor
{
public:
    IStepperMotorMock() : IStepperMotor("IStepperMotorMock")
    {
    }

    MOCK_METHOD(bool, init, (const common::IConfiguration&));
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
