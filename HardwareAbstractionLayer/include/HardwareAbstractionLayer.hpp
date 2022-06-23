///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-01
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IHardwareAbstractionLayer.hpp"

namespace sugo::hal
{
/**
 * @brief Base class of the hardware abstraction layer
 *
 */
class HardwareAbstractionLayer : public IHardwareAbstractionLayer
{
public:
    HardwareAbstractionLayer() : IHardwareAbstractionLayer("hardware-abstraction-layer")
    {
    }

    bool init(const IConfiguration& configuration) override;

    const GpioControllerMap& getGpioControllerMap() const override
    {
        return m_gpioControllerMap;
    }

    const StepperMotorControllerMap& getStepperMotorControllerMap() const override
    {
        return m_stepperMotorControllerMap;
    }

    const TemperatureSensorControllerMap& getTemperatureSensorControllerMap() const override
    {
        return m_temperatureSensorControllerMap;
    }

private:
    GpioControllerMap              m_gpioControllerMap;
    StepperMotorControllerMap      m_stepperMotorControllerMap;
    TemperatureSensorControllerMap m_temperatureSensorControllerMap;
};

}  // namespace sugo::hal
