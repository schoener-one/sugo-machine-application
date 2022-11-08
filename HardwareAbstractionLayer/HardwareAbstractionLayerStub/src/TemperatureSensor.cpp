///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#include "TemperatureSensor.hpp"
#include "Logger.hpp"

using namespace sugo::hal;

TemperatureSensor::~TemperatureSensor()
{
    finalize();
}

bool TemperatureSensor::init(const IConfiguration& configuration)
{
    const auto chipSelect = configuration.getOption("chip-select").get<std::string>();
    LOG(debug) << getId() << ": chip-select pin set to " << chipSelect;
    return true;
}

void TemperatureSensor::finalize()
{
}

TemperatureSensor::Temperature TemperatureSensor::getTemperature() const
{
    return Temperature(0, Unit::Celcius);
}
