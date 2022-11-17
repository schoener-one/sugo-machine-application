
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceControl.hpp"
#include "HardwareAbstractionLayerHelper.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "IMachineControl.hpp"
#include "MachineProtocol.hpp"
#include "MessageProtocol.hpp"
#include "RemoteControlProtocol.hpp"

#include <cassert>
#include <string>

using namespace sugo;
using namespace remote_control;
using namespace sugo::message;

std::string UserInterfaceControl::convertToString(const UserInterfaceControl::Event& event)
{
    switch (event)
    {
        case Event::MachineSwitchedOff:
            return "off";
        case Event::MachineStarting:
            return "starting";
        case Event::MachineRunning:
            return "running";
        case Event::MachineError:
            return "error";
    }
    return "";
}

Json UserInterfaceControl::createStateMessage(const std::string& type)
{
    namespace rp             = remote_control::protocol;
    auto     machineResponse = send(IMachineControl::CommandGetMotorSpeed);
    unsigned speed           = 0;

    if ((machineResponse.result() == CommandResponse_Result_SUCCESS) &&
        !machineResponse.response().empty())
    {
        auto response  = Json::parse(machineResponse.response());
        auto jsonSpeed = response.at(protocol::IdSpeed);
        speed          = (jsonSpeed.empty()) ? 0 : jsonSpeed.get<unsigned>();
    }

    return Json({{rp::IdType, type},
                 {rp::IdResult, rp::IdResultSuccess},
                 {rp::IdState, convertToString(m_lastMachineEvent)},
                 {rp::IdSpeed, speed}});
}

bool UserInterfaceControl::receiveRequest(remote_control::IRequestHandler::ClientId clientId,
                                          const Json& request, Json& response)
{
    namespace rp     = remote_control::protocol;
    const auto& type = request.at(rp::IdType);
    if (type.empty())
    {
        LOG(warning) << clientId << ": Invalid request - missing request type";
        response = {{rp::IdType, rp::IdTypeResponseRequest},
                    {rp::IdResult, rp::IdResultError},
                    {rp::IdErrorReason, rp::IdErrorTypeInvalid}};
        return true;
    }

    const auto typeValue = type.get<std::string>();
    if (typeValue == rp::IdTypeRequestState)
    {
        response = createStateMessage(rp::IdTypeResponseState);
    }
    else if (typeValue == rp::IdTypeRequestCommand)
    {
        if (!request.at(rp::IdCommand).empty())
        {
            const auto command = request.at("command").get<std::string>();
            LOG(debug) << clientId << ": Received command request: " << command;
            message::CommandResponse machineResponse;
            machineResponse.set_result(message::CommandResponse_Result_ERROR);
            if (command == "start")
            {
                machineResponse = send(IMachineControl::CommandSwitchOn);
            }
            else if (command == "stop")
            {
                machineResponse = send(IMachineControl::CommandSwitchOff);
            }
            else if (command == "increase-speed")
            {
                machineResponse = send(IMachineControl::CommandIncreaseMotorSpeed);
            }
            else if (command == "decrease-speed")
            {
                machineResponse = send(IMachineControl::CommandDecreaseMotorSpeed);
            }

            const std::string result =
                machineResponse.result() == message::CommandResponse_Result_SUCCESS
                    ? rp::IdResultSuccess
                    : rp::IdResultError;
            response = {{rp::IdType, rp::IdTypeResponseCommand}, {rp::IdResult, result}};
        }
        else
        {
            response = {{rp::IdType, rp::IdTypeResponseRequest},
                        {rp::IdResult, rp::IdResultError},
                        {rp::IdErrorReason, rp::IdErrorCommandUnsupported}};
        }
    }
    else
    {
        response = {{rp::IdType, rp::IdTypeResponseRequest},
                    {rp::IdResult, rp::IdResultError},
                    {rp::IdErrorReason, rp::IdErrorRequestUnsupported}};
    }
    return true;
}

void UserInterfaceControl::updateMachineState()
{
    namespace rp = remote_control::protocol;

    if (m_cbSendNotification == nullptr)
    {
        LOG(warning) << "No callback for send notification available";
        return;
    }

    m_cbSendNotification(createStateMessage(rp::IdTypeNotificationState));
}

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse UserInterfaceControl::onNotificationMachineControlStarting(
    const message::Command& command)
{
    return handleEventMessage(command, Event::MachineStarting);
}

message::CommandResponse UserInterfaceControl::onNotificationMachineControlRunning(
    const message::Command& command)
{
    return handleEventMessage(command, Event::MachineRunning);
}

message::CommandResponse UserInterfaceControl::onNotificationMachineControlSwitchedOff(
    const message::Command& command)
{
    return handleEventMessage(command, Event::MachineSwitchedOff);
}

message::CommandResponse UserInterfaceControl::onNotificationMachineControlErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event::MachineError);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void UserInterfaceControl::handleMachineStateChange(const Event& event, const State&)

{
    auto& hal          = m_serviceLocator.get<hal::IHardwareAbstractionLayer>();
    m_lastMachineEvent = event;

    switch (event)
    {
        case Event::MachineError:
        // TODO Make LED lights flashing on error!
        case Event::MachineSwitchedOff:
            (void)getGpioPin(hal, hal::id::GpioPinRelaySwitchLightRun)
                ->setState(hal::IGpioPin::State::Low);
            (void)getGpioPin(hal, hal::id::GpioPinRelaySwitchLightReady)
                ->setState(hal::IGpioPin::State::Low);
            break;
        case Event::MachineStarting:
            (void)getGpioPin(hal, hal::id::GpioPinRelaySwitchLightRun)
                ->setState(hal::IGpioPin::State::High);
            (void)getGpioPin(hal, hal::id::GpioPinRelaySwitchLightReady)
                ->setState(hal::IGpioPin::State::Low);
            break;
        case Event::MachineRunning:
            (void)getGpioPin(hal, hal::id::GpioPinRelaySwitchLightRun)
                ->setState(hal::IGpioPin::State::High);
            (void)getGpioPin(hal, hal::id::GpioPinRelaySwitchLightReady)
                ->setState(hal::IGpioPin::State::High);
            break;
    }
    updateMachineState();
}