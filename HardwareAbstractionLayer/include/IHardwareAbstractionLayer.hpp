/*
 * IHardwareAbstractionLayer.hpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#ifndef IHARDWAREABSTRACTIONLAYER_HPP_
#define IHARDWAREABSTRACTIONLAYER_HPP_

#include <cassert>
#include <memory>

#include "IGpioController.hpp"
#include "IStepperMotorController.hpp"

namespace moco
{
class IHardwareAbstractionLayer
{
protected:
    IHardwareAbstractionLayer() {}

public:
    /**
     * Sets the global HAL interface.
     * @param hal HAL interface instance.
     */
    static void init(std::shared_ptr<IHardwareAbstractionLayer> hal) { s_hal = std::move(hal); }

    /**
     * Returns the global HAL interface.
     * @return The global HAL interface.
     */
    static IHardwareAbstractionLayer& get()
    {
        assert(s_hal != nullptr);
        return *s_hal;
    }

public:
    virtual ~IHardwareAbstractionLayer() {}

    virtual IGpioController&               getGpioController()               = 0;
    virtual IStepperMotorController&       getStepperMotorController()       = 0;
    virtual const IStepperMotorController& getStepperMotorController() const = 0;

private:
    static std::shared_ptr<IHardwareAbstractionLayer> s_hal;
};

}  // namespace moco

#endif /* IHARDWAREABSTRACTIONLAYER_HPP_ */
