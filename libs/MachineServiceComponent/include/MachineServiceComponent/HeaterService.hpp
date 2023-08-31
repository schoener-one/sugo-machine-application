///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atomic>
#include <string>

#include "Common/ServiceLocator.hpp"
#include "Common/Timer.hpp"
#include "HardwareAbstractionLayer/IHalObject.hpp"
#include "MachineServiceComponent/HardwareService.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides a heater control service.
class HeaterService : public HardwareService
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
    HeaterService(hal::Identifier heaterId, hal::Identifier temperatureSensorId,
                  const common::ServiceLocator& serviceLocator);

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
    /// @brief Update heater temperature and check.
    void updateHeaterTemperatureAndCheck();

    const hal::Identifier         m_heaterId;             ///< Heater actor identifier.
    const hal::Identifier         m_temperatureSensorId;  ///< Heater temperature sensor identifier.
    const common::ServiceLocator& m_serviceLocator;       ///< Service locator instance.
    common::Timer                 m_temperatureObserver;  ///< Temperature observer timer.
    std::atomic<Temperature>      m_currentTemperature     = 0;  ///< Current measured temperature.
    Temperature                   m_lastCheckedTemperature = 0;  ///< Last measured temperture.
};

}  // namespace sugo::machine_service_component
