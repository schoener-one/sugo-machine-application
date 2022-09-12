//////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-19
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <gmock/gmock.h>

#include "IGpioPin.hpp"

namespace sugo::hal
{
class IGpioPinMock : public IGpioPin
{
public:
    IGpioPinMock() : IGpioPin("IGpioPinMock")
    {
    }

    MOCK_METHOD(bool, init, (const IConfiguration&));
    MOCK_METHOD(State, getState, (), (const));
    MOCK_METHOD(bool, setState, (State));
    MOCK_METHOD(Direction, getDirection, (), (const));
    MOCK_METHOD(Event, waitForEvent, (std::chrono::nanoseconds));
};
}  // namespace sugo::hal
