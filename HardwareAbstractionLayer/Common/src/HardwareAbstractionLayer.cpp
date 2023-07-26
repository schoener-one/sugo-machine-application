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

#include "GpioControl.hpp"
#include "HalHelper.hpp"
#include "IConfiguration.hpp"
#include "StepperMotorControl.hpp"
#include "TemperatureSensorControl.hpp"

#include <limits>
#include <ostream>
#include <string>
#include <vector>

namespace std
{
/**
 * @brief std::ostream type presentation helper function
 */
template <class ValueT>
inline std::ostream& operator<<(std::ostream& ostr, const std::vector<ValueT>& vec)
{
    bool first = true;
    ostr << "[";
    for (const auto& item : vec)
    {
        if (first)
        {
            ostr << "\"" << item << "\"";
            first = false;
        }
        else
        {
            ostr << ", \"" << item << "\"";
        }
    }
    ostr << "]";
    return ostr;
}
}  // namespace std

using namespace sugo::hal;

void HardwareAbstractionLayer::finalize()
{
    postFinalization();
}

bool HardwareAbstractionLayer::init(const IConfiguration& configuration)
{
    if (!initSubComponents<IGpioControl, GpioControl>(configuration, getId(), id::GpioControl,
                                                      m_gpioControllerMap))
    {
        LOG(error) << "Failed to initialize sub components: " << id::GpioControl;
        return false;
    }

    const auto& pinMap = m_gpioControllerMap.at(id::GpioControl)->getGpioPinMap();
    if ((pinMap.count(id::GpioPinMotorControlError) != 1) or
        (pinMap.count(id::GpioPinMotorControlReset) != 1))
    {
        LOG(error) << "Missing mandatory configuration entries";
        return false;
    }

    if (!initSubComponents<ITemperatureSensorControl, TemperatureSensorControl>(
            configuration, getId(), id::TemperatureSensorControl, m_temperatureSensorControllerMap,
            pinMap))
    {
        LOG(error) << "Failed to initialize sub components: " << id::TemperatureSensorControl;
        return false;
    }

    if (!initSubComponents<IStepperMotorControl, StepperMotorControl>(
            configuration, getId(), id::StepperMotorControl, m_stepperMotorControllerMap,
            *pinMap.at(id::GpioPinMotorControlError), *pinMap.at(id::GpioPinMotorControlReset)))
    {
        LOG(error) << "Failed to initialize sub components: " << id::StepperMotorControl;
        return false;
    }

    postInitialization();
    return true;
}

void HardwareAbstractionLayer::addConfigurationOptions(sugo::IConfiguration& configuration)
{
    const static std::string gpioControl{"hardware-abstraction-layer.gpio-control"};
    configuration.add(
        Option(gpioControl + ".device", std::string("gpiochip0"), "HAL: GPIO device name"));
    const static std::string gpioPin{gpioControl + ".gpio-pin"};
    // clang-format off
    configuration.add(Option(gpioPin + "-enabled",  std::vector<std::string>{},         "HAL: List of GPIO pin names to be enabled", true));
    for (const auto& name : {".relay-switch-heater-feeder", ".relay-switch-heater-merger", ".relay-switch-light-power",
        ".relay-switch-light-run", ".relay-switch-light-ready", ".signal-filament-tension-overload", ".signal-button-stop",
        ".signal-filament-tension-low", ".signal-filament-tension-high", ".temperature-sensor-control-cs-merger",
        ".temperature-sensor-control-cs-feeder", ".motor-control-error", ".motor-control-reset", ".relay-switch-fan-feeder",
        ".relay-switch-fan-merger"})
    {
        configuration.add(Option(gpioPin + name + ".pin",         IGpioPin::InvalidPin, "HAL: GPIO pin number"));
        configuration.add(Option(gpioPin + name + ".direction",   std::string("in"),    "HAL: GPIO direction (in, out)"));
        configuration.add(Option(gpioPin + name + ".active-high", true,                 "HAL: GPIO active-high status (true, false)"));
        configuration.add(Option(gpioPin + name + ".state",       std::string(""),      "HAL: GPIO default state (high, low)"));
    }
    // StepperMotorControl
    const static std::string stepperMotorControl = "hardware-abstraction-layer.stepper-motor-control";
    configuration.add(
        Option(stepperMotorControl + ".device", std::string("i2c-1"), "HAL: Motor control I2C device name"));
    const static std::string motor = stepperMotorControl + ".motor";
    configuration.add(Option(motor + "-enabled",  std::vector<std::string>{}, "HAL: List of stepper-motor names to be enabled", true));
    for (const auto& name : {".feeder", ".coiler"})
    {
        configuration.add(Option(motor + name + ".i2c-address",   static_cast<unsigned>(std::numeric_limits<uint8_t>::max()), "HAL: Motor I2C address"));
        configuration.add(Option(motor + name + ".direction",     std::string("forward"), "HAL: Motor rotation direction (forward, backward)"));
        configuration.add(Option(motor + name + ".max-speed-rpm", 0u,                     "HAL: Motor rotation maximum speed [rpm]"));
    }
    // TemperatureSensorControl
    const static std::string temperatureSensorControl = "hardware-abstraction-layer.temperature-sensor-control";
    configuration.add(Option(temperatureSensorControl + ".device", std::string("spidev0.0"), "HAL: ADC device name"));
    const static std::string temperatureSensor = "hardware-abstraction-layer.temperature-sensor-control.temperature-sensor";
    configuration.add(Option(temperatureSensor + "-enabled",  std::vector<std::string>{}, "HAL: List of temperature sensors enabled", true));
    for (const auto& name : {".temperature-sensor-feeder", ".temperature-sensor-merger"})
    {
        configuration.add(Option(temperatureSensor + name + ".chip-select",  std::string(""), "HAL: Chip select pin"));
    }
    // clang-format on
}
