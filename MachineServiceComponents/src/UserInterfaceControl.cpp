
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
#include "IHardwareAbstractionLayer.hpp"
#include "IMachineControl.hpp"

#include <cassert>
#include <string>

using namespace sugo;
using namespace hal;
using namespace remote_control;

namespace
{
const std::string GpioControl("gpio-control");
const std::string GpioPinRelaySwitchLightRun("relay-switch-light-run");
const std::string GpioPinRelaySwitchLightPower("relay-switch-light-power");
const std::string GpioPinRelaySwitchLightReady("relay-switch-light-ready");

inline auto& getGpioPin(const ServiceLocator& serviceLocator, const std::string& gpioPinName)
{
    auto& gpioController =
        serviceLocator.get<IHardwareAbstractionLayer>().getGpioControllerMap().at(GpioControl);
    assert(gpioController);
    auto& gpioPin = gpioController->getGpioPinMap().at(gpioPinName);
    assert(gpioPin);
    return gpioPin;
}

constexpr const char* convertToString(UserInterfaceControl::State state)
{
    switch (state)
    {
        case UserInterfaceControl::State::Off:
            return "off";
        case UserInterfaceControl::State::Starting:
            return "starting";
        case UserInterfaceControl::State::Running:
            return "running";
        case UserInterfaceControl::State::Error:
            return "error";
    }
    return "";
}
}  // namespace

Json UserInterfaceControl::createStateMessage(const std::string& type)
{
    auto     machineResponse = send(IMachineControl::CommandGetMotorSpeed);
    auto     response        = Json::parse(machineResponse.response());
    auto     jsonSpeed       = response.at("speed");
    unsigned speed           = (jsonSpeed.empty()) ? 0 : jsonSpeed.get<unsigned>();
    return Json({{"type", type},
                 {"result", "ok"},
                 {"state", convertToString(getCurrentState())},
                 {"speed", speed}});
}

bool UserInterfaceControl::receiveRequest(remote_control::IRequestHandler::ClientId clientId,
                                          const Json& request, Json& response)
{
    const auto type = request.at("type");
    if (type.empty())
    {
        LOG(warning) << clientId << ": Invalid request - missing request type";
        response.push_back(
            {{"type", "request-response"}, {"result", "nok"}, {"reason", "invalid-type"}});
        return true;
    }

    const auto typeValue = type.get<std::string>();
    if (typeValue == "state-request")
    {
        response.push_back(createStateMessage("state-response"));
    }
    else if (typeValue == "command-request")
    {
        if (!request.at("command").empty())
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
                machineResponse.result() == message::CommandResponse_Result_SUCCESS ? "nok" : "ok";
            response.push_back({{"type", "command-response"}, {"result", result}});
        }
        else
        {
            response.push_back(
                {{"type", "request-response"}, {"result", "nok"}, {"reason", "invalid-command"}});
        }
    }
    else
    {
        response.push_back(
            {{"type", "request-response"}, {"result", "nok"}, {"reason", "invalid-request"}});
    }
    return true;
}

void UserInterfaceControl::updateMachineState()
{
    if (m_cbSendNotification == nullptr)
    {
        LOG(warning) << "No callback for send notification available";
        return;
    }
    m_cbSendNotification(createStateMessage("state-notification"));
}

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse UserInterfaceControl::onCommandMachineControlStarting(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse UserInterfaceControl::onCommandMachineControlRunning(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartingFinished));
}

message::CommandResponse UserInterfaceControl::onCommandMachineControlSwitchedOff(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void UserInterfaceControl::switchOff(const IUserInterfaceControl::Event&,
                                     const IUserInterfaceControl::State&)
{
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightRun)->setState(IGpioPin::State::Low);
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightReady)
        ->setState(IGpioPin::State::Low);
    updateMachineState();
}

void UserInterfaceControl::switchOn(const IUserInterfaceControl::Event&,
                                    const IUserInterfaceControl::State&)
{
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightRun)->setState(IGpioPin::State::High);
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightReady)
        ->setState(IGpioPin::State::Low);
    updateMachineState();
}

void UserInterfaceControl::switchRunning(const IUserInterfaceControl::Event&,
                                         const IUserInterfaceControl::State&)
{
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightRun)->setState(IGpioPin::State::High);
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightReady)
        ->setState(IGpioPin::State::High);
    updateMachineState();
}
