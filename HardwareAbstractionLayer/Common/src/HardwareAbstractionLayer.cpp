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
#include "StepperMotorControl.hpp"
#include "HalHelper.hpp"

using namespace sugo::hal;

bool HardwareAbstractionLayer::init(const IConfiguration& configuration)
{
    bool success = initSubComponents<IGpioControl, GpioControl>(
        configuration, getId(), "gpio-control", m_gpioControllerMap);
    if (success)
    {
        initSubComponents<IAdcControl, AdcControl>(configuration, getId(), "adc-control",
                                                   m_adcControllerMap);
    }
    if (success)
    {
        initSubComponents<IStepperMotorControl, StepperMotorControl>(
            configuration, getId(), "stepper-motor-control", m_stepperMotorControllerMap);
    }
    return success;
}
