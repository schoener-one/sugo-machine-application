
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-15
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ServiceLocator.hpp"
#include "Timer.hpp"

#include <atomic>

namespace sugo
{
/**
 * @class Class represents a heater
 */
class HeaterService
{
public:
    // Constructor / Destructor
    explicit HeaterService(const ServiceLocator& serviceLocator)
        : m_serviceLocator(serviceLocator),
          m_temperatureObserver([this]() { this->updateHeaterTemperature(); })
    {
    }
    virtual ~HeaterService()
    {
    }

protected:
    bool         switchHeater(const std::string& heaterSwitchId, bool switchOn);
    void         updateHeaterTemperature();
    virtual void onMaxTemperatureReached() = 0;
    virtual void onMinTemperatureReached() = 0;
    int32_t      getTemperature() const
    {
        return m_lastTemperature.load();
    }
    bool startTemperatureObservation();
    void stopTemperatureObservation();

private:
    const ServiceLocator& m_serviceLocator;
    Timer                 m_temperatureObserver;
    std::atomic_int32_t   m_lastTemperature = 0;
};

}  // namespace sugo
