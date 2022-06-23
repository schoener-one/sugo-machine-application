///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#include "HardwareAbstractionLayer.hpp"

#include "Configuration.hpp"
#include "GpioControl.hpp"
#include "HalHelper.hpp"
#include "StepperMotorControl.hpp"
#include "TemperatureSensorControl.hpp"

using namespace sugo::hal;

bool HardwareAbstractionLayer::init(const IConfiguration& configuration)
{
    bool success = initSubComponents<IGpioControl, GpioControl>(
        configuration, getId(), "gpio-control", m_gpioControllerMap);
    if (success)
    {
        const auto& pinMap = m_gpioControllerMap.at("gpio-control")->getGpioPinMap();
        success      = (pinMap.count("motor-control-error") == 1) &&
                  (pinMap.count("motor-control-reset") == 1);
        if (success)
        {
            success = initSubComponents<ITemperatureSensorControl, TemperatureSensorControl>(
                configuration, getId(), "temperature-sensor-control",
                m_temperatureSensorControllerMap, pinMap);
        }

        if (success)
        {
            success = initSubComponents<IStepperMotorControl, StepperMotorControl>(
                configuration, getId(), "stepper-motor-control", m_stepperMotorControllerMap,
                *pinMap.at("motor-control-error"), *pinMap.at("motor-control-reset"));
        }
    }

    return success;
}
