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

using namespace sugo;

HeaterService::HeaterService(const std::string& heaterId, const std::string& temperatureSensorId,
                             const ServiceLocator& serviceLocator)
    : m_heaterId(heaterId),
      m_temperatureSensorId(temperatureSensorId),
      m_serviceLocator(serviceLocator),
      m_temperatureObserver(
          [this]() {
              this->updateHeaterTemperature(
                  std::bind(&HeaterService::onMinTemperatureReached, this),
                  std::bind(&HeaterService::onMaxTemperatureReached, this));
          },
          heaterId + "Timer")
{
}

bool HeaterService::switchHeater(bool switchOn)
{
    auto pinHeaterMerger =
        getGpioPin(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), m_heaterId);
    return pinHeaterMerger->setState(switchOn ? hal::IGpioPin::State::High
                                              : hal::IGpioPin::State::Low);
}

void HeaterService::updateHeaterTemperature(TemperatureRangeHandler minTemperatureReached,
                                            TemperatureRangeHandler maxTemperatureReached)
{
    auto& temperatureSensor = getTemperatureSensor(
        m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), m_temperatureSensorId);
    auto value = temperatureSensor->getTemperature();
    assert(value.getUnit() == hal::Unit::Celcius);

    const Temperature lastTemperature = m_lastTemperature;
    m_lastTemperature                 = value.getValue();

    // Implement hysteresis
    if ((lastTemperature < value.getValue()) && (value.getValue() >= config::MaxHeaterTemperature))
    {
        // Is heating up
        LOG(debug) << "Max temperature reached: " << lastTemperature << "/" << value.getValue();
        if (maxTemperatureReached)
        {
            maxTemperatureReached();
        }
    }
    else if ((lastTemperature > value.getValue()) &&
             (value.getValue() <= config::MinHeaterTemperature))
    {
        // Is heating down
        LOG(debug) << "Min temperature reached: " << lastTemperature << "/" << value.getValue();
        if (minTemperatureReached)
        {
            minTemperatureReached();
        }
    }
}

bool HeaterService::startTemperatureObservation()
{
    return m_temperatureObserver.start(config::TemperatureObservationPeriod);
}

void HeaterService::stopTemperatureObservation()
{
    m_temperatureObserver.stop();
}