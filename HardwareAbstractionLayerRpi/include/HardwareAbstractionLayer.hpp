/*
 * HardwareAbstractionLayer.hpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#ifndef HARDWAREABSTRACTIONLAYER_HPP_
#define HARDWAREABSTRACTIONLAYER_HPP_

#include <memory>

#include "IConfiguration.hpp"
#include "IHardwareAbstractionLayer.hpp"

namespace moco
{
class HardwareAbstractionLayer : public IHardwareAbstractionLayer
{
public:
    static void setConfiguration(IConfiguration& configuration);

    explicit HardwareAbstractionLayer(const IConfiguration& configuration);
    virtual ~HardwareAbstractionLayer();

    // IHardwareAbstractionLayer {{
    IGpioController& getGpioController() override { return *m_gpioController; }

    IStepperMotorController& getStepperMotorController() override
    {
        return *m_stepperMotorController;
    }

    const IStepperMotorController& getStepperMotorController() const override
    {
        return *m_stepperMotorController;
    }
    // IHardwareAbstractionLayer }}

private:
    std::unique_ptr<IGpioController>         m_gpioController;
    std::unique_ptr<IStepperMotorController> m_stepperMotorController;
};

}  // namespace moco

#endif /* HARDWAREABSTRACTIONLAYER_HPP_ */
