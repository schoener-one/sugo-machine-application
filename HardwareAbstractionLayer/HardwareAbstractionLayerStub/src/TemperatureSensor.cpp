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
#include "Simulator.hpp"

using namespace sugo::hal;

TemperatureSensor::~TemperatureSensor()
{
    finalize();
}

bool TemperatureSensor::init(const IConfiguration& configuration)
{
    const auto chipSelect = configuration.getOption(option::id::ChipSelect).get<std::string>();
    LOG(debug) << getId() << "." << option::id::ChipSelect << ": " << chipSelect;

    return true;
}

void TemperatureSensor::finalize()
{
}

TemperatureSensor::Temperature TemperatureSensor::getTemperature() const
{
    return Simulator::getInstance().getTemperature(getId());
}
