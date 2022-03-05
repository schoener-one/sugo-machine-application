/*
 * HardwareAbstractionLayerImpl.cpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#include "HardwareAbstractionLayer.hpp"

#include "AdcControl.hpp"
#include "Configuration.hpp"
#include "GpioControl.hpp"
#include "StepperMotorControl.hpp"
#include "Logger.hpp"

using namespace sugo::hal;

template <class IControlT, class ControlT>
bool HardwareAbstractionLayer::initControl(
    const IConfiguration& configuration, const std::string& controlId,
    std::map<IHalObject::Identifier, std::unique_ptr<IControlT>>& map)
{
    Configuration controlConfig;
    configuration.extract(std::string("hardware-abstraction-layer.") + controlId + ".",
                          controlConfig);
    auto control = std::make_unique<ControlT>(controlId);

    if (!control->init(controlConfig))
    {
        LOG(error) << "failed to initialize " << controlId;
        return false;
    }
    map.insert({controlId, std::move(control)});
    return true;
}

bool HardwareAbstractionLayer::init(const IConfiguration& configuration)
{
    initControl<IGpioControl, GpioControl>(configuration, "gpio-control", m_gpioControllerMap);
    initControl<IAdcControl, AdcControl>(configuration, "adc-control", m_adcControllerMap);
    initControl<IStepperMotorControl, StepperMotorControl>(configuration, "stepper-motor-control",
                                                           m_stepperMotorControllerMap);

    return true;
}
