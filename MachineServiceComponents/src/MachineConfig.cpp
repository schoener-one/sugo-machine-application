///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-15
 */
///////////////////////////////////////////////////////////////////////////////

#include "MachineConfig.hpp"

using namespace sugo;

void MachineConfig::addConfigurationOptions(sugo::IConfiguration& configuration)
{
    configuration.add(Option(id::MotorSpeedDefault, static_cast<unsigned>(50),
                             "MachineControl: Default motor speed."));
    configuration.add(Option(id::MotorSpeedMax, static_cast<unsigned>(100),
                             "MachineControl: Maximum motor speed."));
    configuration.add(Option(id::MotorSpeedIncrement, static_cast<unsigned>(10),
                             "MachineControl: Motor speed increment."));
    configuration.add(Option(id::HeaterTemperatureMax, static_cast<int>(205),
                             "MachineControl: Maximum heater temperature."));
    configuration.add(Option(id::HeaterTemperatureMin, static_cast<int>(195),
                             "MachineControl: Minimum heater temperature."));
    configuration.add(Option(id::ObservationTimeoutGpioPin, static_cast<unsigned>(1000u),
                             "MachineControl: Observation timeout for GPIO pins."));
    configuration.add(Option(id::ObservationTimeoutTemperature, static_cast<unsigned>(1000u),
                             "MachineControl: Observation timeout for temperature values."));
    configuration.add(Option(id::ObservationTimeoutTension, static_cast<unsigned>(1000u),
                             "MachineControl: Observation timeout for filament tension values."));
}