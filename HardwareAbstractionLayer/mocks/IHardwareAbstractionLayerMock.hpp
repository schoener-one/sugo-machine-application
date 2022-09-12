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

#include "IHardwareAbstractionLayer.hpp"

namespace sugo::hal
{
class IHardwareAbstractionLayerMock : public IHardwareAbstractionLayer
{
public:
    IHardwareAbstractionLayerMock() : IHardwareAbstractionLayer("IHardwareAbstractionLayerMock")
    {
    }

    MOCK_METHOD(bool, init, (const IConfiguration&));
    MOCK_METHOD(const GpioControllerMap&, getGpioControllerMap, (), (const));
    MOCK_METHOD(const StepperMotorControllerMap&, getStepperMotorControllerMap, (), (const));
    MOCK_METHOD(const TemperatureSensorControllerMap&, getTemperatureSensorControllerMap, (),
                (const));
};
}  // namespace sugo::hal
