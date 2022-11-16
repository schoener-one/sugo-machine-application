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
#include "Logger.hpp"
#include "MachineConfig.hpp"

#include <cassert>
#include <limits>

using namespace sugo;

HeaterService::HeaterService(hal::Identifier heaterId, hal::Identifier temperatureSensorId,
                             const ServiceLocator& serviceLocator)
    : m_heaterId(std::move(heaterId)),
      m_temperatureSensorId(std::move(temperatureSensorId)),
      m_serviceLocator(serviceLocator),
      m_temperatureObserver(
          config::TemperatureObservationPeriod, [&]() { updateHeaterTemperatureAndCheck(); },
          m_heaterId + "Timer"),
      m_lastCheckedTemperature(std::numeric_limits<Temperature>::min())
{
}

bool HeaterService::switchHeater(bool switchOn)
{
    const auto& pinHeaterMerger =
        getGpioPin(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), m_heaterId);
    LOG(debug) << "Switch heater " << m_heaterId << " " << (switchOn ? "on" : "off");
    return pinHeaterMerger->setState(switchOn ? hal::IGpioPin::State::High
                                              : hal::IGpioPin::State::Low);
}

void HeaterService::updateHeaterTemperature()
{
    auto& temperatureSensor = getTemperatureSensor(
        m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), m_temperatureSensorId);
    auto value = temperatureSensor->getTemperature();
    assert(value.getUnit() == hal::Unit::Celcius);
    m_currentTemperature = value.getValue();
}

void HeaterService::updateHeaterTemperatureAndCheck()
{
    updateHeaterTemperature();

    if (m_lastCheckedTemperature != m_currentTemperature)
    {
        // Implement hysteresis
        if (m_currentTemperature >= config::MaxHeaterTemperature)
        {
            LOG(debug) << "Max temperature reached: " << m_lastCheckedTemperature << "/"
                       << m_currentTemperature;
            onTemperatureLimitEvent(TemperatureLimitEvent::MaxTemperatureReached);
        }
        else if (m_currentTemperature <= config::MinHeaterTemperature)
        {
            LOG(debug) << "Min temperature reached: " << m_lastCheckedTemperature << "/"
                       << m_currentTemperature;
            onTemperatureLimitEvent(TemperatureLimitEvent::MinTemperatureReached);
        }

        m_lastCheckedTemperature = m_currentTemperature;
    }
}

bool HeaterService::startTemperatureObservation()
{
    return m_temperatureObserver.start();
}

void HeaterService::stopTemperatureObservation()
{
    m_temperatureObserver.stop();
}