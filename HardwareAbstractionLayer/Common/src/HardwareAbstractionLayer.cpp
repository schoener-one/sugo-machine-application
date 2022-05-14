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

#include "AdcControl.hpp"
#include "Configuration.hpp"
#include "GpioControl.hpp"
#include "HalHelper.hpp"
#include "StepperMotorControl.hpp"

using namespace sugo::hal;

bool HardwareAbstractionLayer::init(const IConfiguration& configuration)
{
    bool success = initSubComponents<IGpioControl, GpioControl>(
        configuration, getId(), "gpio-control", m_gpioControllerMap);
    if (success)
    {
        auto& pinMap = m_gpioControllerMap.at("gpio-control")->getGpioPinMap();
        success      = (pinMap.count("adc-control-chipselect") == 1) &&
                  (pinMap.count("adc-control-data-ready") == 1) &&
                  (pinMap.count("adc-control-reset") == 1) &&
                  (pinMap.count("motor-control-error") == 1) &&
                  (pinMap.count("motor-control-reset") == 1);
        if (success)
        {
            success = initSubComponents<IAdcControl, AdcControl>(
                configuration, getId(), "adc-control", m_adcControllerMap,
                *pinMap.at("adc-control-chipselect"), *pinMap.at("adc-control-reset"),
                *pinMap.at("adc-control-data-ready"));
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
