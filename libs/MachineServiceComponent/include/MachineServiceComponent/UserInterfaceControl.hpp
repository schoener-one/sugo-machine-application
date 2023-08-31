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
#include "Common/Thread.hpp"
#include "Common/Types.hpp"
#include "MachineServiceComponent/UserLightService.hpp"
#include "RemoteControl/IClientRequestHandler.hpp"
#include "ServiceComponent/IUserInterfaceControl.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides control of the user interface unit.
class UserInterfaceControl : public service_component::IUserInterfaceControl,
                             public remote_control::IClientRequestHandler
{
public:
    /**
     * @brief Construct a new user interface control object.
     *
     * @param messageBroker Message broker instance.
     * @param processContext Process context.
     * @param serviceLocator Service locator instance.
     */
    UserInterfaceControl(message_broker::IMessageBroker& messageBroker,
                         common::IProcessContext&        processContext,
                         const common::ServiceLocator&   serviceLocator);

    /**
     * @brief Receives a HTTP request from the client.
     *
     * @param clientId Client identifier.
     * @param request Request to be processed.
     * @param response[out] Reponse to be sent.
     * @return true If the request could be processed successfully.
     * @return false If the request could not be processed successfully.
     */
    bool receiveRequest(remote_control::IClientRequestHandler::ClientId clientId,
                        const common::Json& request, common::Json& response) override;

    /**
     * @brief Registers a send notification callback.
     *
     * @param callback Callback to be register.
     */
    void registerSendNotification(SendNotificationCallback callback) override
    {
        m_cbSendNotification = callback;
    }

protected:
    // Request handlers
    void onNotificationMachineControlStopped(const message_broker::Message& request) override;
    void onNotificationMachineControlHeatingUp(const message_broker::Message& request) override;
    void onNotificationMachineControlStarting(const message_broker::Message& request) override;
    void onNotificationMachineControlRunning(const message_broker::Message& request) override;
    void onNotificationMachineControlSwitchedOff(const message_broker::Message& request) override;
    void onNotificationMachineControlErrorOccurred(const message_broker::Message& request) override;

    // Transition actions
    void handleMachineStateChange(const Event& event, const State& state) override;

private:
    /// @brief Updates the state machine.
    void updateMachineState();

    /**
     * @brief Creates a new state message in JSON format.
     *
     * @param type Current state type.
     * @return common::Json Created state message in JSON format.
     */
    common::Json createStateMessage(const std::string& type);

    /**
     * @brief Converts a user interface event to a string.
     *
     * @param event Event to be converted.
     * @return std::string Resulting event string.
     */
    static std::string convertToString(const UserInterfaceControl::Event& event);

    UserLightService m_userLightService;  ///< User light service. // TODO Pass services as
                                          ///< interfaces from outside!
    SendNotificationCallback m_cbSendNotification = nullptr;  ///< Notification send callback.
    Event m_lastMachineEvent = Event::MachineSwitchedOff;     ///< Last machine event.
};

}  // namespace sugo::machine_service_component
