
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
#include "MachineServiceComponent/HeaterService.hpp"
#include "ServiceComponent/IFilamentMergerHeater.hpp"

namespace sugo::machine_service_component
{
/// @brief Class represents a filament merger heater.
class FilamentMergerHeater : public service_component::IFilamentMergerHeater, public HeaterService
{
public:
    /**
     * @brief Construct a new filament merger heater object.
     *
     * @param messageBroker Message broker instance.
     * @param processContext Process context.
     * @param serviceLocator Service locator instance.
     */
    FilamentMergerHeater(message_broker::IMessageBroker& messageBroker,
                         common::IProcessContext&        processContext,
                         const common::ServiceLocator&   serviceLocator);

protected:
    // Request handlers
    message_broker::ResponseMessage onPropertyRequestGetTemperature(
        const message_broker::Message& request) override;

    // Transition actions
    void stopHeating(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void startHeating(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

    // HeaterService events
    void onTemperatureLimitEvent(TemperatureLimitEvent event) override;
};

}  // namespace sugo::machine_service_component
