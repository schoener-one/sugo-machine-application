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

#include "Common/ServiceLocator.hpp"
#include "HardwareAbstractionLayer/Identifier.hpp"
#include "MachineServiceComponent/HardwareService.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides a heater control service.
class UserLightService : public HardwareService
{
public:
    /**
     * @brief Creates a new user light service object
     *
     * @param powerLightId Identifier for power light.
     * @param runLightId Identifier for run light.
     * @param readyLightId Identifier for ready light.
     * @param serviceLocator Service locator to retrieve the HAL.
     */
    UserLightService(hal::Identifier powerLightId, hal::Identifier runLightId,
                     hal::Identifier readyLightId, const common::ServiceLocator& serviceLocator);

    /**
     * @brief Switches the user lights on or off.
     *
     * @param powerLightOn Indicates if power light should be switched on.
     * @param runLightOn Indicates if run light should be switched on.
     * @param readyLightOn Indicates if ready light should be switched on.
     */
    void switchLight(bool powerLightOn, bool runLightOn, bool readyLightOn);

private:
    const hal::Identifier m_powerLightId;  ///< Power light id.
    const hal::Identifier m_runLightId;    ///< Power light id.
    const hal::Identifier m_readyLightId;  ///< Power light id.
};

}  // namespace sugo::machine_service_component
