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

#include <cassert>
#include <string>

#include "HardwareAbstractionLayer/IHardwareAbstractionLayer.hpp"
#include "MachineServiceComponent/Protocol.hpp"
#include "MachineServiceComponent/UserInterfaceControl.hpp"
#include "MessageBroker/Message.hpp"
#include "RemoteControl/Protocol.hpp"
#include "ServiceComponent/IMachineControl.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

UserInterfaceControl::UserInterfaceControl(message_broker::IMessageBroker& messageBroker,
                                           common::IProcessContext&        processContext,
                                           const common::ServiceLocator&   serviceLocator)
    : IUserInterfaceControl(messageBroker, processContext),
      m_userLightService(hal::id::GpioPinRelaySwitchLightPower, hal::id::GpioPinRelaySwitchLightRun,
                         hal::id::GpioPinRelaySwitchLightReady, serviceLocator)
{
}

std::string UserInterfaceControl::convertToString(const UserInterfaceControl::Event& event)
{
    switch (event)
    {
        case Event::MachineSwitchedOff:
            return "off";
        case Event::MachineStopped:
            return "stopped";
        case Event::MachineHeatingUp:
        case Event::MachineStarting:
            return "starting";
        case Event::MachineRunning:
            return "running";
        case Event::MachineError:
            return "error";
    }
    return "";
}

common::Json UserInterfaceControl::createStateMessage(const std::string& type)
{
    namespace rp                          = remote_control::id;
    unsigned                        speed = 0;
    message_broker::ResponseMessage machineResponse{};
    const auto                      success = send(IMachineControl::RequestGetMotorSpeed);

    if (success && !machineResponse.getPayload().empty())
    {
        auto response  = common::Json::parse(machineResponse.getPayload());
        auto jsonSpeed = response.at(id::Speed);
        speed          = (jsonSpeed.empty()) ? 0 : jsonSpeed.get<unsigned>();
    }

    return common::Json({{rp::Type, type},
                         {rp::Result, rp::ResultSuccess},
                         {rp::State, convertToString(m_lastMachineEvent)},
                         {rp::Speed, speed}});
}

bool UserInterfaceControl::receiveRequest(remote_control::IClientRequestHandler::ClientId clientId,
                                          const common::Json& request, common::Json& response)
{
    namespace rp     = remote_control::id;
    const auto& type = request.at(rp::Type);
    if (type.empty())
    {
        LOG(warning) << clientId << ": Invalid request - missing request type";
        response = {{rp::Type, rp::TypeResponseRequest},
                    {rp::Result, rp::ResultError},
                    {rp::ErrorReason, rp::ErrorTypeInvalid}};
        return true;
    }

    const auto typeValue = type.get<std::string>();
    if (typeValue == rp::TypeRequestState)
    {
        response = createStateMessage(rp::TypeResponseState);
    }
    else if (typeValue == rp::TypeRequestCommand)
    {
        if (!request.at(rp::Command).empty())
        {
            const auto requestId = request.at("command").get<std::string>();
            LOG(debug) << clientId << ": Received request: " << requestId;
            message_broker::ResponseMessage machineResponse{};
            machineResponse.setResult(message_broker::ResponseMessage::Result::Success);
            bool success = false;

            if (requestId == "switch-on")
            {
                success = send(IMachineControl::RequestSwitchOn, machineResponse);
            }
            else if (requestId == "start")
            {
                success = send(IMachineControl::RequestStart, machineResponse);
            }
            else if (requestId == "start-heatless")
            {
                success = send(IMachineControl::RequestStartHeatless, machineResponse);
            }
            else if (requestId == "stop")
            {
                success = send(IMachineControl::RequestStop, machineResponse);
            }
            else if (requestId == "increase-speed")
            {
                success = send(IMachineControl::RequestIncreaseMotorSpeed, machineResponse);
            }
            else if (requestId == "decrease-speed")
            {
                success = send(IMachineControl::RequestDecreaseMotorSpeed, machineResponse);
            }
            else if (requestId == "switch-off")
            {
                success = send(IMachineControl::RequestSwitchOff, machineResponse);
            }

            const std::string result = success ? rp::ResultSuccess : rp::ResultError;
            response                 = {{rp::Type, rp::TypeResponseCommand}, {rp::Result, result}};
        }
        else
        {
            response = {{rp::Type, rp::TypeResponseRequest},
                        {rp::Result, rp::ResultError},
                        {rp::ErrorReason, rp::ErrorCommandUnsupported}};
        }
    }
    else
    {
        response = {{rp::Type, rp::TypeResponseRequest},
                    {rp::Result, rp::ResultError},
                    {rp::ErrorReason, rp::ErrorRequestUnsupported}};
    }
    return true;
}

void UserInterfaceControl::updateMachineState()
{
    namespace rp = remote_control::id;

    if (m_cbSendNotification == nullptr)
    {
        LOG(warning) << "No callback for send notification available";
        return;
    }

    m_cbSendNotification(createStateMessage(rp::TypeNotificationState));
}

///////////////////////////////////////////////////////////////////////////////
// Requests:

void UserInterfaceControl::onNotificationMachineControlStarting(
    const message_broker::Message& request)
{
    (void)handleEventMessage(request, Event::MachineStarting);
}

void UserInterfaceControl::onNotificationMachineControlStopped(
    const message_broker::Message& request)
{
    (void)handleEventMessage(request, Event::MachineStopped);
}

void UserInterfaceControl::onNotificationMachineControlHeatingUp(
    const message_broker::Message& request)
{
    (void)handleEventMessage(request, Event::MachineHeatingUp);
}

void UserInterfaceControl::onNotificationMachineControlRunning(
    const message_broker::Message& request)
{
    (void)handleEventMessage(request, Event::MachineRunning);
}

void UserInterfaceControl::onNotificationMachineControlSwitchedOff(
    const message_broker::Message& request)
{
    (void)handleEventMessage(request, Event::MachineSwitchedOff);
}

void UserInterfaceControl::onNotificationMachineControlErrorOccurred(
    const message_broker::Message& request)
{
    (void)handleEventMessage(request, Event::MachineError);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void UserInterfaceControl::handleMachineStateChange(const Event& event, const State&)
{
    m_lastMachineEvent = event;

    switch (event)
    {
        case Event::MachineError:
        // TODO Make LED lights flashing on error!
        case Event::MachineSwitchedOff:
            m_userLightService.switchLight(false, false, false);
            break;
        case Event::MachineStopped:
            m_userLightService.switchLight(true, false, false);
            break;
        case Event::MachineHeatingUp:
        case Event::MachineStarting:
            m_userLightService.switchLight(true, true, false);
            break;
        case Event::MachineRunning:
            m_userLightService.switchLight(true, true, true);
            break;
    }
    updateMachineState();
}