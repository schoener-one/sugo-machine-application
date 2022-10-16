
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
#include <string>

namespace sugo
{
/**
 * @class Class represents a heater
 */
class HeaterService
{
public:
    /// @brief Temperature value type
    using Temperature = int32_t;

    /**
     * @brief Constructs a new heater service object.
     *
     * @param heaterId            Heater id to be used.
     * @param temperatureSensorId Temperature sensor id to be used.
     * @param serviceLocator      Service locator to retrieve the HAL.
     */
    explicit HeaterService(const std::string& heaterId, const std::string& temperatureSensorId,
                           const ServiceLocator& serviceLocator);
    virtual ~HeaterService() = default;

protected:
    /**
     * @brief
     *
     * @param switchOn Indicates if the heater should be switched on or off.
     * @return true    If the heater could be switched successfully.
     * @return false   If the heater could not be switched successfully.
     */
    bool switchHeater(bool switchOn);

    /**
     * @brief Updates the current temperature and calls the appropriate handler callbacks.
     */
    void updateHeaterTemperature();
    /**
     * @brief Called if the max temperature range has been reached.
     */
    virtual void onMaxTemperatureReached() = 0;
    /**
     * @brief Called if the min temperature range has been reached.
     */
    virtual void onMinTemperatureReached() = 0;
    int32_t      getTemperature() const
    {
        return m_currentTemperature.load();
    }
    bool startTemperatureObservation();
    void stopTemperatureObservation();

private:
    void updateHeaterTemperatureAndCheck();

    const std::string        m_heaterId;
    const std::string        m_temperatureSensorId;
    const ServiceLocator&    m_serviceLocator;
    Timer                    m_temperatureObserver;
    std::atomic<Temperature> m_currentTemperature     = 0;
    Temperature              m_lastCheckedTemperature = 0;
};

}  // namespace sugo
