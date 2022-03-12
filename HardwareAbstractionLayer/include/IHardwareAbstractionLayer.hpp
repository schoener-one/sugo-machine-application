///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2019-10-13
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include <memory>
#include <string>

#include "IAdcControl.hpp"
#include "IConfiguration.hpp"
#include "IGpioControl.hpp"
#include "IHalObject.hpp"
#include "IStepperMotorControl.hpp"

namespace sugo::hal
{
/**
 * @brief Interface class for the hardware abstraction layer
 *
 */
class IHardwareAbstractionLayer : public IHalObject
{
public:
    /// GPIO controller map
    using GpioControllerMap = std::map<IHalObject::Identifier, std::unique_ptr<IGpioControl>>;
    /// Stepper motor controller map
    using StepperMotorControllerMap =
        std::map<IHalObject::Identifier, std::unique_ptr<IStepperMotorControl>>;
    /// ADC input controller map
    using AdcControllerMap = std::map<IHalObject::Identifier, std::unique_ptr<IAdcControl>>;

    /**
     * @brief Destroy the IHardwareAbstractionLayer object
     *
     */
    virtual ~IHardwareAbstractionLayer()
    {
    }

    /**
     * @brief Set the Configuration object.
     *
     * @param configuration Configuration to be set.
     */
    static void setConfiguration(IConfiguration& configuration);

    /**
     * @brief Returns a map of available GPIO controllers.
     *
     * @return const GpioControllerMap& Map of available controllers.
     */
    virtual const GpioControllerMap& getGpioControllerMap() const = 0;

    /**
     * @brief Returns a map of available stepper motor controllers.
     *
     * @return const StepperMotorControllerMap& Map of available controllers.
     */
    virtual const StepperMotorControllerMap& getStepperMotorControllerMap() const = 0;

    /**
     * @brief Returns a map of available ADC controllers.
     *
     * @return const AdcControllerMap& Map of available controllers.
     */
    virtual const AdcControllerMap& getAdcControllerMap() const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
