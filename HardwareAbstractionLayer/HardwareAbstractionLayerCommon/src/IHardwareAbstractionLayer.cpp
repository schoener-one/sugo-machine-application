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

#include "IHardwareAbstractionLayer.hpp"

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
    const static std::string              gpioPins = "hardware-abstraction-layer.gpio-control.pins";
    const static std::vector<std::string> emptyList = {};
    // clang-format off
    configuration.add(Option(gpioPins + "-enabled",  emptyList,         "HAL: List of GPIO pin names to be enabled", true));
    for (const auto& name : {".adc-control-data-ready", ".adc-control-reset", ".adc-control-chipselect"})
    {
        configuration.add(Option(gpioPins + name + ".pin",         -1,                "HAL: GPIO pin number"));
        configuration.add(Option(gpioPins + name + ".direction",   std::string("in"), "HAL: GPIO direction (in, out)"));
        configuration.add(Option(gpioPins + name + ".active-high", true,              "HAL: GPIO active-high status (true, false)"));
    }
    const static std::string motors = "hardware-abstraction-layer.stepper-motor-control.motors";
    configuration.add(Option(motors + "-enabled",  emptyList, "HAL: List of stepper-motor names to be enabled", true));
    for (const auto& name : {".feeder", ".coil"})
    {
        configuration.add(Option(motors + name + ".i2c-id",     -1,                     "HAL: Motor I2C identifier"));
        configuration.add(Option(motors + name + ".direction",  std::string("forward"), "HAL: Motor rotation direction (forward, backward)"));
        configuration.add(Option(motors + name + ".speed",      0u,                     "HAL: Motor rotation speed [rpm]"));
    }
    const static std::string adcInputs = "hardware-abstraction-layer.adc-control.inputs";
    configuration.add(Option(adcInputs + "-enabled",  emptyList, "HAL: List of stepper-motor names to be enabled", true));
    for (const auto& name : {".temperature-feeder", ".temperature-merger", ".temperature-hot-filament", ".temperature-cool-filament", ".tension-filament"})
    {
        configuration.add(Option(adcInputs + name + ".id",        -1,            "HAL: ADC input identifier"));
        configuration.add(Option(adcInputs + name + ".filter",  std::string(""), "HAL: ADC value filter"));
    }
    // clang-format on
}
