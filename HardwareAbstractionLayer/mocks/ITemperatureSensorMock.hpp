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

#include "ITemperatureSensor.hpp"

namespace sugo::hal
{
class ITemperatureSensorMock : public ITemperatureSensor
{
public:
    ITemperatureSensorMock() : ITemperatureSensor("ITemperatureSensorMock")
    {
    }

    MOCK_METHOD(bool, init, (const IConfiguration&));
    MOCK_METHOD(Temperature, getTemperature, (), (const));
};
}  // namespace sugo::hal
