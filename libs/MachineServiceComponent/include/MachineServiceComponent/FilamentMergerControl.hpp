
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
#include "MachineServiceComponent/ControlService.hpp"
#include "ServiceComponent/IFilamentMergerControl.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides control of the filament merger unit.
class FilamentMergerControl : public service_component::IFilamentMergerControl,
                              public ControlService<FilamentMergerControl>
{
public:
    /**
     * @brief Construct a new filament merger control object.
     *
     * @param messageBroker Message broker instance.
     * @param processContext Process context.
     * @param serviceLocator Service locator instance.
     */
    FilamentMergerControl(message_broker::IMessageBroker& messageBroker,
                          common::IProcessContext&        processContext,
                          const common::ServiceLocator&   serviceLocator);

protected:
    // Request handlers
    void onNotificationFilamentPreHeaterTargetTemperatureRangeReached(
        const message_broker::Message& request) override;
    void onNotificationFilamentMergerHeaterTargetTemperatureRangeReached(
        const message_broker::Message& request) override;

    // Transition actions
    void switchOff(const Event& event, const State& state) override;
    void stopMerger(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void startMotor(const Event& event, const State& state) override;
    void notifyHeatedUp(const Event& event, const State& state) override;
    void heatingUp(const Event& event, const State& state) override;
    void notifyRunning(const Event& event, const State& state) override;

private:
    /// @brief Switches off the unit.
    void switchOff();

    const common::ServiceLocator& m_serviceLocator;  ///< Service locator instance.
    bool                          m_isPreHeaterTemperatureReached =
        false;  ///< Indicates if pre heating temperature is reached.
    bool m_isMergerHeaterTemperatureReached =
        false;  ///< Indicates if merger heater temperature is reached.

    friend class ControlService<FilamentMergerControl>;
};

}  // namespace sugo::machine_service_component
