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

#include "IGpioControl.hpp"

namespace sugo::hal
{
class IGpioControlMock : public IGpioControl
{
public:
    IGpioControlMock() : IGpioControl("IGpioControlMock")
    {
    }

    MOCK_METHOD(bool, init, (const IConfiguration&));
    MOCK_METHOD(const GpioPinMap&, getGpioPinMap, ());
};
}  // namespace sugo::hal
