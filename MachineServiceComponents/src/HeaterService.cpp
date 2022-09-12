///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-15
 */
///////////////////////////////////////////////////////////////////////////////

#include "HeaterService.hpp"
#include "HardwareAbstractionLayerHelper.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "MachineConfig.hpp"
#include "Logger.hpp"

#include <cassert>

using namespace sugo;

bool HeaterService::switchHeater(const std::string& heaterSwitchId, bool switchOn)
{
    auto pinHeaterMerger =
        getGpioPin(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), heaterSwitchId);
    return pinHeaterMerger->setState(switchOn ? hal::IGpioPin::State::High
                                              : hal::IGpioPin::State::Low);
}

void HeaterService::updateHeaterTemperature()
{
    auto& temperatureSensor = getTemperatureSensor(
        m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), config::TemperatureSensorMergerId);
    auto value = temperatureSensor->getTemperature();
    assert(value.getUnit() == hal::Unit::Celcius);

    // Implement hysteresis
    if ((m_lastTemperature < value.getValue()) && (value.getValue() >= config::MaxHeaterTemperature))
    {
        // Is heating up
        LOG(debug) << "Max temperature reached: " << m_lastTemperature << "/" << value.getValue();
        onMaxTemperatureReached();
    }
    else if ((m_lastTemperature > value.getValue()) && (value.getValue() <= config::MinHeaterTemperature))
    {
        // Is heating down
        LOG(debug) << "Min temperature reached: " << m_lastTemperature << "/" << value.getValue();
        onMinTemperatureReached();
    }
    m_lastTemperature = value.getValue();
}

bool HeaterService::startTemperatureObservation()
{
    return m_temperatureObserver.start(config::TemperatureObservationPeriod);
}

void HeaterService::stopTemperatureObservation()
{
    m_temperatureObserver.stop();
}