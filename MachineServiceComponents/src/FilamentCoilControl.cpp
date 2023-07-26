
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentCoilControl.hpp"
#include "IFilamentCoilMotor.hpp"
#include "IFilamentTensionSensor.hpp"
#include "MachineConfig.hpp"
#include "MachineProtocol.hpp"

using namespace sugo;
using namespace sugo::message;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentCoilControl::onCommandSwitchOn(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOn);
}

message::CommandResponse FilamentCoilControl::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOff);
}

message::CommandResponse FilamentCoilControl::onCommandStartCoil(const message::Command& command)
{
    const auto  parameters     = Json::parse(command.parameters());
    const auto& tensionControl = parameters.at(protocol::IdTensionControl);

    if (tensionControl.empty())
    {
        return createErrorCommandResponse(
            command, Json({{message::protocol::IdErrorReason,
                            message::protocol::IdErrorCommandParameterInvalid}}));
    }

    m_controlTension = tensionControl.get<bool>();
    return handleEventMessage(command, Event::StartMotor);
}

message::CommandResponse FilamentCoilControl::onCommandStopCoil(const message::Command& command)
{
    return handleEventMessage(command, Event::StopMotor);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorTensionTooLow(
    const message::Command& command)
{
    return handleEventMessage(command, Event::TensionTooLow);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorTensionTooHigh(
    const message::Command& command)
{
    return handleEventMessage(command, Event::TensionTooHigh);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorTensionOverloaded(
    const message::Command& command)
{
    return handleEventMessage(command, Event::TensionOverloaded);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event::ErrorOccurred);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorStartMotorSucceeded(
    const message::Command& command)
{
    return handleEventMessage(command, Event::StartMotorSucceeded);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorStopMotorSucceeded(
    const message::Command& command)
{
    return handleEventMessage(command, Event::StopMotorSucceeded);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event::ErrorOccurred);
}

message::CommandResponse FilamentCoilControl::onCommandSetMotorSpeed(
    const message::Command& command)
{
    return forward(IFilamentCoilMotor::CommandSetMotorSpeed, command);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentCoilControl::switchOn(const IFilamentCoilControl::Event&,
                                   const IFilamentCoilControl::State&)
{
    const auto responseSwitchOnCoilMotor = send(IFilamentCoilMotor::CommandSwitchOn);
    if (responseSwitchOnCoilMotor.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::SwitchOnFailed);
        return;
    }

    const auto responseSwitchOnTensionSensor = send(IFilamentTensionSensor::CommandSwitchOn);
    if (responseSwitchOnTensionSensor.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::SwitchOnFailed);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void FilamentCoilControl::switchOff(const IFilamentCoilControl::Event&,
                                    const IFilamentCoilControl::State&)
{
    switchOff();
}

void FilamentCoilControl::switchOff()
{
    switchOffServiceComponent<IFilamentCoilMotor>();
    switchOffServiceComponent<IFilamentTensionSensor>();
}

void FilamentCoilControl::handleError(const IFilamentCoilControl::Event&,
                                      const IFilamentCoilControl::State&)
{
    switchOff();
    notify(NotificationErrorOccurred);
}

void FilamentCoilControl::stopMotor(const IFilamentCoilControl::Event&,
                                    const IFilamentCoilControl::State&)
{
    const auto response = send(IFilamentCoilMotor::CommandStopMotor);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::ErrorOccurred);
    }
    notify(NotificationCoilStopped);
}

void FilamentCoilControl::startMotor(const IFilamentCoilControl::Event&,
                                     const IFilamentCoilControl::State&)
{
    const auto response = send(IFilamentCoilMotor::CommandStartMotor);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::ErrorOccurred);
    }
}

void FilamentCoilControl::notifyRunning(const IFilamentCoilControl::Event&,
                                        const IFilamentCoilControl::State&)
{
    notify(NotificationCoilRunning);
}

void FilamentCoilControl::controlFilamentTension(const IFilamentCoilControl::Event& event,
                                                 const IFilamentCoilControl::State&)
{
    if (m_controlTension)
    {
        message::CommandResponse response;

        if (event == Event::TensionTooLow)
        {
            response = send(IFilamentCoilMotor::CommandIncreaseMotorOffsetSpeed);
        }
        else if (event == Event::TensionTooHigh)
        {
            response = send(IFilamentCoilMotor::CommandDecreaseMotorOffsetSpeed);
        }

        if (response.result() != CommandResponse_Result_SUCCESS)
        {
            push(Event::ErrorOccurred);
        }
    }
    else
    {
        LOG(debug) << "Ignoring tension event " << event
                   << " because tension control is turned off";
    }
}