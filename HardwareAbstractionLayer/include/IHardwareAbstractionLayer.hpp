/*
 * IHardwareAbstractionLayer.hpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#pragma once

#include <cassert>
#include <memory>

#include "IConfiguration.hpp"
#include "IGpioController.hpp"
#include "IStepperMotorController.hpp"

namespace sugo
{
class IHardwareAbstractionLayer
{
protected:
    IHardwareAbstractionLayer() {}

public:
    /**
     * @brief Set the Configuration object.
     *
     * @param configuration Configuration to be set.
     */
    static void setConfiguration(IConfiguration& configuration);

    /**
     * Returns the global HAL interface.
     * @return The global HAL interface.
     */
    static IHardwareAbstractionLayer& get();

public:
    virtual ~IHardwareAbstractionLayer() {}

    virtual void                           init(const IConfiguration& configuration) = 0;
    virtual IGpioController&               getGpioController()                       = 0;
    virtual IStepperMotorController&       getStepperMotorController()               = 0;
    virtual const IStepperMotorController& getStepperMotorController() const         = 0;
};

}  // namespace sugo
