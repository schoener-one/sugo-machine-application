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
#include "Max31865.hpp"

#include <cassert>

using namespace sugo::hal;

TemperatureSensor::~TemperatureSensor()
{
    finalize();
}

bool TemperatureSensor::init(const IConfiguration& configuration)
{
    assert(m_driver == nullptr);

    const std::string chipSelect = configuration.getOption("chip-select").get<std::string>();
    LOG(debug) << getId() << ": chip-select set to " << chipSelect;

    auto& ioCs = m_gpioPins.at(chipSelect);
    if (!ioCs)
    {
        LOG(error) << getId() << ": failed to get chip-select pin";
        return false;
    }

    m_driver = new Max31865(m_spiControl, *ioCs);
    if (!m_driver->init())
    {
        LOG(error) << getId() << ": failed to init sensor driver";
        finalize();
        return false;
    }

    return true;
}

void TemperatureSensor::finalize()
{
    if (m_driver != nullptr)
    {
        delete m_driver;
        m_driver = nullptr;
    }
}

TemperatureSensor::Temperature TemperatureSensor::getTemperature() const
{
    assert(m_driver != nullptr);

    return Temperature(m_driver->getTemperature(), Unit::Celcius);
}
