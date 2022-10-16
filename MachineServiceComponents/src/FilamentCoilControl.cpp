
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
    return handleStateChangeMessage(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentCoilControl::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentCoilControl::onCommandStartCoil(const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::StartMotor));
}

message::CommandResponse FilamentCoilControl::onCommandStopCoil(const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::StopMotor));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorTensionTooLow(
    const message::Command& command)
{
    m_motorOffsetSpeed = ((m_motorOffsetSpeed - config::MotorSpeedInc) % config::MaxMotorSpeed);
    send(IFilamentCoilMotor::CommandSetMotorOffsetSpeed,
         Json({{protocol::IdSpeed, m_motorOffsetSpeed}}));
    return createCommandResponse(command);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorTensionTooHigh(
    const message::Command& command)
{
    m_motorOffsetSpeed = ((m_motorOffsetSpeed + config::MotorSpeedInc) % config::MaxMotorSpeed);
    send(IFilamentCoilMotor::CommandSetMotorOffsetSpeed,
         Json({{protocol::IdSpeed, m_motorOffsetSpeed}}));
    return createCommandResponse(command);
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentTensionSensorErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::ErrorOccurred));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorStartMotorSucceeded(
    const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::StartMotorSucceeded));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorStartMotorFailed(
    const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::StartMotorFailed));
}

message::CommandResponse FilamentCoilControl::onNotificationFilamentCoilMotorErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::ErrorOccurred));
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
    m_motorOffsetSpeed                   = 0;
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

    const auto responseSetMotorOffsetSpeed = send(IFilamentCoilMotor::CommandSetMotorOffsetSpeed,
                                                  Json({{protocol::IdSpeed, m_motorOffsetSpeed}}));
    if (responseSetMotorOffsetSpeed.result() == message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::SwitchOnSucceeded));
    }
    else
    {
        push(Event(EventId::SwitchOnFailed));
    }
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
