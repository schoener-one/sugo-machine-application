
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
    return handleEventMessage(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentCoilControl::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentCoilControl::onCommandStartCoil(const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::StartMotor));
}

message::CommandResponse FilamentCoilControl::onCommandStopCoil(const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::StopMotor));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorTensionTooLow(
    const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::TensionTooLow));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorTensionTooHigh(
    const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::TensionTooHigh));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::ErrorOccurred));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorStartMotorSucceeded(
    const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::StartMotorSucceeded));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorStartMotorFailed(
    const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::StartMotorFailed));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::ErrorOccurred));
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
        push(Event(EventId::SwitchOnFailed));
        return;
    }

    const auto responseSwitchOnTensionSensor = send(IFilamentTensionSensor::CommandSwitchOn);
    if (responseSwitchOnTensionSensor.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::SwitchOnFailed));
        return;
    }

    push(Event(EventId::SwitchOnSucceeded));
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
    if (response.result() == message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::StopMotorSucceeded));
    }
    else
    {
        push(Event(EventId::StopMotorFailed));
    }
    notify(NotificationCoilStopped);
}

void FilamentCoilControl::startMotor(const IFilamentCoilControl::Event&,
                                     const IFilamentCoilControl::State&)
{
    const auto response = send(IFilamentCoilMotor::CommandStartMotor);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::StartMotorFailed));
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
    message::CommandResponse response;

    if (event == EventId::TensionTooLow)
    {
        response = send(IFilamentCoilMotor::CommandIncreaseMotorOffsetSpeed,
                        Json({{protocol::IdSpeed, config::MotorSpeedInc}}));
    }
    else if (event == EventId::TensionTooHigh)
    {
        response = send(IFilamentCoilMotor::CommandDecreaseMotorOffsetSpeed,
                        Json({{protocol::IdSpeed, config::MotorSpeedInc}}));
    }

    if (response.result() != CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::ErrorOccurred));
    }
}