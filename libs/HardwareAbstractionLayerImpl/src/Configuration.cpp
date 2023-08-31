///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <ostream>

#include "HardwareAbstractionLayer/Configuration.hpp"
#include "HardwareAbstractionLayer/IGpioPin.hpp"

namespace std
{
/// @brief std::ostream type presentation helper function
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

void config::addConfigurationOptions(sugo::common::IConfiguration& configuration)
{
    configuration.add(common::Option(id::ConfigGpioControlDevice, def::ConfigGpioControlDevice,
                                     description::ConfigGpioControlDevice));
    configuration.add(common::Option(id::ConfigGpioPinEnabled, def::ConfigGpioPinEnabled,
                                     description::ConfigGpioPinEnabled, true));

    // clang-format off
    // TODO move names to identifiers!
    for (const auto& name : {".relay-switch-heater-feeder", ".relay-switch-heater-merger", ".relay-switch-light-power",
        ".relay-switch-light-run", ".relay-switch-light-ready", ".signal-filament-tension-overload", ".signal-button-stop",
        ".signal-filament-tension-low", ".signal-filament-tension-high", ".temperature-sensor-control-cs-merger",
        ".temperature-sensor-control-cs-feeder", ".motor-control-error", ".motor-control-reset", ".relay-switch-fan-feeder",
        ".relay-switch-fan-merger"})
    {
        configuration.add(common::Option(id::ConfigGpioPin + name + ".pin",         IGpioPin::InvalidPin, "GPIO pin number"));
        configuration.add(common::Option(id::ConfigGpioPin + name + ".direction",   std::string("in"),    "GPIO direction (in, out)"));
        configuration.add(common::Option(id::ConfigGpioPin + name + ".active-high", true,                 "GPIO active-high status (true, false)"));
        configuration.add(common::Option(id::ConfigGpioPin + name + ".state",       std::string(""),      "GPIO default state (high, low)"));
    }
    // clang-format on

    // StepperMotorControl
    configuration.add(common::Option(id::ConfigStepperMotorControlDevice,
                                     def::ConfigStepperMotorControlDevice,
                                     description::ConfigStepperMotorControlDevice));
    configuration.add(common::Option(id::ConfigStepperMotorEnabled, def::ConfigStepperMotorEnabled,
                                     description::ConfigStepperMotorEnabled, true));

    // clang-format off
    // TODO move names to identifiers!
    for (const auto& name : {".feeder", ".coiler"})
    {
        configuration.add(common::Option(id::ConfigStepperMotor + name + ".i2c-address",   static_cast<unsigned>(std::numeric_limits<uint8_t>::max()), "Motor I2C address"));
        configuration.add(common::Option(id::ConfigStepperMotor + name + ".direction",     std::string("forward"), "Motor rotation direction (forward, backward)"));
        configuration.add(common::Option(id::ConfigStepperMotor + name + ".max-speed-rpm", 0u,                     "Motor rotation maximum speed [rpm]"));
    }
    // clang-format on

    // TemperatureSensorControl
    configuration.add(common::Option(id::ConfigTemperatureSensorDevice,
                                     def::ConfigTemperatureSensorDevice,
                                     description::ConfigTemperatureSensorDevice));
    configuration.add(common::Option(id::ConfigTemperatureSensorEnabled,
                                     def::ConfigTemperatureSensorEnabled,
                                     description::ConfigTemperatureSensorEnabled, true));

    // clang-format off
    // TODO move names to identifiers!
    for (const auto& name : {".temperature-sensor-feeder", ".temperature-sensor-merger"})
    {
        configuration.add(common::Option(id::ConfigTemperatureSensor + name + ".chip-select",  std::string(""), "Chip select pin"));
    }
    // clang-format on
}
