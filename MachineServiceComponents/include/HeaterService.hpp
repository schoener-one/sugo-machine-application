
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

#include "IHalObject.hpp"
#include "ServiceLocator.hpp"
#include "Timer.hpp"

#include <atomic>
#include <string>

namespace sugo
{
/**
 * @class Class provides a heater control service.
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
    explicit HeaterService(const hal::Identifier& heaterId,
                           const hal::Identifier& temperatureSensorId,
                           const ServiceLocator&  serviceLocator);
    virtual ~HeaterService() = default;

protected:
    /**
     * @brief Temperature limit event.
     *
     */
    enum TemperatureLimitEvent
    {
        MaxTemperatureReached,
        MinTemperatureReached
    };

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
     * @brief Called if a new temperature limit event has occurred.
     */
    virtual void onTemperatureLimitEvent(TemperatureLimitEvent event) = 0;

    /**
     * @brief Returns the current temperature.
     *
     * @return The current temperature.
     */
    int32_t getTemperature() const
    {
        return m_currentTemperature.load();
    }

    /**
     * @brief Starts the temperature sensor observation.
     *
     * @return true  If observation could be started successfully.
     * @return false If observation could not be started successfully.
     */

    bool startTemperatureObservation();

    /**
     * @brief Stops the temperature sensor observation.
     *
     */
    void stopTemperatureObservation();

private:
    void updateHeaterTemperatureAndCheck();

    const hal::Identifier    m_heaterId;
    const hal::Identifier    m_temperatureSensorId;
    const ServiceLocator&    m_serviceLocator;
    Timer                    m_temperatureObserver;
    std::atomic<Temperature> m_currentTemperature     = 0;
    Temperature              m_lastCheckedTemperature = 0;
};

}  // namespace sugo
