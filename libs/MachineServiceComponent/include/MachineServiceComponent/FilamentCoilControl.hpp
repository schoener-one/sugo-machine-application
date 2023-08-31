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
#include "ServiceComponent/IFilamentCoilControl.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides control of the filament coil unit.
class FilamentCoilControl : public service_component::IFilamentCoilControl,
                            public ControlService<FilamentCoilControl>
{
public:
    /**
     * @brief Construct a new filament coil control object.
     *
     * @param messageBroker Message broker instance.
     * @param processContext Process context.
     * @param serviceLocator Service locator instance.
     */
    FilamentCoilControl(message_broker::IMessageBroker& messageBroker,
                        common::IProcessContext&        processContext,
                        const common::ServiceLocator&   serviceLocator);

protected:
    // Request handlers
    message_broker::ResponseMessage onRequestStartCoil(
        const message_broker::Message& request) override;

    // Transition actions
    void switchOff(const Event& event, const State& state) override;
    void startMotor(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void stopMotor(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void notifyRunning(const Event& event, const State& state) override;
    void controlFilamentTension(const Event& event, const State& state) override;

private:
    void switchOff();

    const common::ServiceLocator& m_serviceLocator;
    bool m_controlTension = false;  // Indicates if tension control is on or off

    friend class ControlService<FilamentCoilControl>;
};

}  // namespace sugo::machine_service_component
