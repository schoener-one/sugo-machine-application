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

#include "IGpioControl.hpp"
#include "IHalObject.hpp"
#include "IStepperMotorControl.hpp"
#include "ITemperatureSensorControl.hpp"

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
    using GpioControllerMap = std::map<Identifier, std::shared_ptr<IGpioControl>>;
    /// Stepper motor controller map
    using StepperMotorControllerMap = std::map<Identifier, std::shared_ptr<IStepperMotorControl>>;
    /// ADC input controller map
    using TemperatureSensorControllerMap =
        std::map<Identifier, std::shared_ptr<ITemperatureSensorControl>>;

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
     * @return const TemperatureSensorControllerMap& Map of available controllers.
     */
    virtual const TemperatureSensorControllerMap& getTemperatureSensorControllerMap() const = 0;

protected:
    using IHalObject::IHalObject;
};

}  // namespace sugo::hal
