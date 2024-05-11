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
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/HardwareService.hpp"
#include "MachineServiceComponent/Types.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides a heater control service.
class HeaterService : public HardwareService
{
public:
    /**
     * @brief Constructs a new heater service object.
     *
     * @param heaterId            Heater id to be used.
     * @param temperatureSensorId Temperature sensor id to be used.
     * @param serviceLocator      Service locator to retrieve the HAL.
     */
    HeaterService(hal::Identifier heaterId, hal::Identifier temperatureSensorId,
                  machine_service_component::Identifier minTemperatureId,
                  machine_service_component::Identifier maxTemperatureId,
                  const common::ServiceLocator&         serviceLocator);

protected:
    /**
     * @brief Temperature limit event.
     *
     */
    enum TemperatureState
    {
        BetweenMinMaxTemperature,  ///< Temperature between min and max.
        AboveMaxTemperature,       ///< Temperature is above max temperature limit.
        BelowMinTemperature,       ///< Temperature is below min temperature limit.
        ErrorNoTemperature         ///< Error: no valid temperature could be retrieved.
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
     * @brief Updates the current heater temperature and calls the appropriate handler callbacks.
     * @return true    If the temperature could be retrieved.
     * @return false   If the temperature could not be retrieved.
     */
    bool updateHeaterTemperature();

    /**
     * @brief Calculates the current heater temperature state based on the current temperature.
     *
     * @return TemperatureState The current temperature state.
     */
    TemperatureState getHeaterTemperatureState();

    /**
     * @brief Returns the current heater temperature.
     *
     * @return The current temperature.
     */
    int32_t getHeaterTemperature() const
    {
        return m_currentTemperature.load();
    }

private:
    const hal::Identifier m_heaterId;             ///< Heater actor identifier.
    const hal::Identifier m_temperatureSensorId;  ///< Heater temperature sensor identifier.
    const machine_service_component::Identifier m_minTemperatureId;  ///< Min temperature id.
    const machine_service_component::Identifier m_maxTemperatureId;  ///< Max temperature id.
    const common::ServiceLocator&               m_serviceLocator;    ///< Service locator instance.
    std::atomic<Temperature>                    m_currentTemperature =
        InvalidTemperature;  ///< Current measured temperature.
};

}  // namespace sugo::machine_service_component
