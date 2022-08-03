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

#include "IStepperMotorControl.hpp"

namespace sugo::hal
{
class IStepperMotorControlMock : public IStepperMotorControl
{
public:
    IStepperMotorControlMock() : IStepperMotorControl("IStepperMotorControlMock")
    {
    }
    MOCK_METHOD(bool, init, (const IConfiguration&));

    MOCK_METHOD(void, reset, ());
    MOCK_METHOD(const StepperMotorMap&, getStepperMotorMap, ());
};
}  // namespace sugo::hal
