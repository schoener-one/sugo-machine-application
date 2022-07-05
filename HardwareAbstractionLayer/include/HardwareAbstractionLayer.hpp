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

#include "IConfiguration.hpp"
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

    /**
     * @brief Extract the hardware configuration from existing configuration.
     *
     * @param[in] configuration Source configuration
     * @param[out] extractedConfiguration Extracted hardware configuration.
     * @return const sugo::IConfiguration& The extracted hardware configuration which was passed.
     */
    const sugo::IConfiguration& getHardwareConfiguration(
        const sugo::IConfiguration& configuration, sugo::IConfiguration& extractedConfiguration);

    /**
     * @brief Set the Configuration object.
     *
     * @param configuration Configuration to be set.
     */
    static void setConfiguration(sugo::IConfiguration& configuration);

private:
    GpioControllerMap              m_gpioControllerMap;
    StepperMotorControllerMap      m_stepperMotorControllerMap;
    TemperatureSensorControllerMap m_temperatureSensorControllerMap;
};

}  // namespace sugo::hal
