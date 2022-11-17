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

#ifdef HAL_SIMULATION_ENABLE
#include <memory>

namespace sugo::hal
{
class Simulator;
}  // namespace sugo::hal
#endif  // HAL_SIMULATION_ENABLE

namespace sugo::hal
{
/**
 * @brief Base class of the hardware abstraction layer
 *
 */
class HardwareAbstractionLayer : public IHardwareAbstractionLayer
{
public:
    HardwareAbstractionLayer();
    ~HardwareAbstractionLayer() override;

    bool init(const IConfiguration& configuration) override;
    void finalize();

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
     * @brief Set the Configuration options.
     *
     * @param configuration Configuration object to which the options will be added.
     */
    static void addConfigurationOptions(sugo::IConfiguration& configuration);

private:
    void postInitialization();
    void postFinalization();

    GpioControllerMap              m_gpioControllerMap;
    StepperMotorControllerMap      m_stepperMotorControllerMap;
    TemperatureSensorControllerMap m_temperatureSensorControllerMap;
#ifdef HAL_SIMULATION_ENABLE
    std::unique_ptr<Simulator> m_simulator;
#endif  // HAL_SIMULATION_ENABLE
};

}  // namespace sugo::hal
