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

#include "ITemperatureSensorControl.hpp"

namespace sugo::hal
{
class ITemperatureSensorControlMock : public ITemperatureSensorControl
{
public:
    ITemperatureSensorControlMock() : ITemperatureSensorControl("ITemperatureSensorControlMock")
    {
    }

    MOCK_METHOD(bool, init, (const IConfiguration&));
    MOCK_METHOD(const TemperatureSensorMap&, getTemperatureSensorMap, ());
};
}  // namespace sugo::hal
