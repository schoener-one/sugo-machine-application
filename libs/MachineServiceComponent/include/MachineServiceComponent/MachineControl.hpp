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
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/ControlService.hpp"
#include "ServiceComponent/IMachineControl.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides control of the machine unit.
class MachineControl : public service_component::IMachineControl,
                       public ControlService<MachineControl>
{
public:
    /**
     * @brief Construct a new machine control object.
     *
     * @param messageBroker Message broker instance.
     * @param processContext Process context.
     * @param serviceLocator Service locator instance.
     */
    MachineControl(message_broker::IMessageBroker& messageBroker,
                   common::IProcessContext&        processContext,
                   const common::ServiceLocator&   serviceLocator);

protected:
    // Request handlers
    message_broker::ResponseMessage onCommandRequestIncreaseMotorSpeed(
        const message_broker::Message& request) override;
    message_broker::ResponseMessage onCommandRequestDecreaseMotorSpeed(
        const message_broker::Message& request) override;

    // Notification handler actions
    void checkStartingState() override;
    void checkStoppingState() override;

    // Transition actions
    void switchOn(const Event& event, const State& state) override;
    void startHeating(const Event& event, const State& state) override;
    void stopMachine(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void notifyStopped(const Event& event, const State& state) override;
    void startMachine(const Event& event, const State& state) override;

private:
    void switchOff();

    const common::ServiceLocator& m_serviceLocator;  ///< Service locator instance.
    bool                          m_isFilamentMergerControlRunning =
        false;  ///< Indicates if filament merger control unit is running.
    bool m_isFilamentCoilControlRunning =
        false;  ///< Indicates if filament coil control unit is running.
    friend class ControlService<MachineControl>;  // FIXME: use another solution here!
};

}  // namespace sugo::machine_service_component
