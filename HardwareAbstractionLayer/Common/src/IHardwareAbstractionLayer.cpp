///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-02-20
 */
///////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <string>
#include <vector>

#include "IGpioPin.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "IStepperMotor.hpp"
#include "IAdcInput.hpp"

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

void IHardwareAbstractionLayer::setConfiguration(IConfiguration& configuration)
{
    const static std::string gpioControl{"hardware-abstraction-layer.gpio-control"};
    configuration.add(Option(gpioControl + ".device",   std::string("gpiochip0"),    "HAL: GPIO device name"));
    const static std::string gpioPin{gpioControl + ".gpio-pin"};
    // clang-format off
    configuration.add(Option(gpioPin + "-enabled",  std::vector<std::string>{},         "HAL: List of GPIO pin names to be enabled", true));
    for (const auto& name : {".relay-switch-heater-feeder", ".relay-switch-heater-merger", ".relay-switch-light-on",
        ".relay-switch-light-run", ".signal-button-start", ".signal-button-stop", ".adc-control-chipselect",
        ".adc-control-data-ready", ".adc-control-reset"})
    {
        configuration.add(Option(gpioPin + name + ".pin",         IGpioPin::InvalidPin, "HAL: GPIO pin number"));
        configuration.add(Option(gpioPin + name + ".direction",   std::string("in"),    "HAL: GPIO direction (in, out)"));
        configuration.add(Option(gpioPin + name + ".active-high", true,                 "HAL: GPIO active-high status (true, false)"));
        configuration.add(Option(gpioPin + name + ".state",       std::string(""),      "HAL: GPIO default state (high, low)"));
    }
    const static std::string motor = "hardware-abstraction-layer.stepper-motor-control.motor";
    configuration.add(Option(motor + "-enabled",  std::vector<std::string>{}, "HAL: List of stepper-motor names to be enabled", true));
    for (const auto& name : {".feeder", ".coiler"})
    {
        configuration.add(Option(motor + name + ".i2c-id",        IStepperMotor::InvalidI2cId, "HAL: Motor I2C identifier"));
        configuration.add(Option(motor + name + ".direction",     std::string("forward"),      "HAL: Motor rotation direction (forward, backward)"));
        configuration.add(Option(motor + name + ".max-speed-rpm", 0u,                          "HAL: Motor rotation maximum speed [rpm]"));
    }
    // ADC
    const static std::string adcControl = "hardware-abstraction-layer.adc-control";
    configuration.add(Option(adcControl + ".device", std::string("spidev0.0"), "HAL: ADC device name"));
    const static std::string adc = "hardware-abstraction-layer.adc-control.adc";
    configuration.add(Option(adc + "-enabled",  std::vector<std::string>{}, "HAL: List of ADC channels to be enabled", true));
    for (const auto& name : {".temperature-feeder", ".temperature-merger", ".test-resistance-100k", ".test-resistance-none"})
    {
        configuration.add(Option(adc + name + ".channel",     0xffffffffu,     "HAL: ADC channel identifier"));
        configuration.add(Option(adc + name + ".adc-filter",  std::string(""), "HAL: ADC value filter"));
    }
    const static std::string adcFilter = "hardware-abstraction-layer.adc-control.adc-filter";
    configuration.add(Option(adcFilter + "-enabled",  std::vector<std::string>{}, "HAL: List of ADC filters to be enabled", true));
    for (const auto& name : {".ntc-temperature", ".voltage-divider", ".voltage"})
    {
        configuration.add(Option(adcFilter + name + ".ref-voltage",                      0, "HAL: Reference voltage [mV]"));
        configuration.add(Option(adcFilter + name + ".translation",        std::string(""), "HAL: Translation to be used"));
        configuration.add(Option(adcFilter + name + ".resistance",                      0u, "HAL: Resistance [mO]"));
        configuration.add(Option(adcFilter + name + ".divider-resistance",              0u, "HAL: Divider resistance [mO]"));
    }
    // clang-format on
}
