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

#include <memory>

#include "IHalObject.hpp"
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
    /**
     * @brief Initializes the hardware abstraction layer
     *
     * @param configuration Configuration to be applied
     * @return true  If could be initialized successfully
     * @return false If could not be initialized successfully
     */
    bool init(const IConfiguration& configuration);

    const GpioControllerMap& getGpioControllerMap() const override
    {
        return m_gpioControllerMap;
    }

    const StepperMotorControllerMap& getStepperMotorControllerMap() const override
    {
        return m_stepperMotorControllerMap;
    }

    const AdcControllerMap& getAdcControllerMap() const override
    {
        return m_adcControllerMap;
    }

private:
    template <class IControlT, class ControlT>
    static bool initControl(const IConfiguration& configuration, const std::string& controlId,
                            std::map<IHalObject::Identifier, std::unique_ptr<IControlT>>& map);

    GpioControllerMap         m_gpioControllerMap;
    StepperMotorControllerMap m_stepperMotorControllerMap;
    AdcControllerMap          m_adcControllerMap;
};

}  // namespace sugo::hal
