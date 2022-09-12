
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

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentCoilControl::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentCoilControl::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentCoilControl::onCommandStartCoil(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotor));
}

message::CommandResponse FilamentCoilControl::onCommandStopCoil(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StopMotor));
}

message::CommandResponse FilamentCoilControl::onCommandFilamentTensionSensorTensionTooLow(
    const message::Command& command)
{
    m_motorOffsetSpeed = ((m_motorOffsetSpeed - config::MotorSpeedInc) % config::MaxMotorSpeed);
    send(IFilamentCoilMotor::CommandSetMotorOffsetSpeed, Json({{protocol::IdSpeed, m_motorOffsetSpeed}}));
    return createResponse(command);
}

message::CommandResponse FilamentCoilControl::onCommandFilamentTensionSensorTensionTooHigh(
    const message::Command& command)
{
    m_motorOffsetSpeed = ((m_motorOffsetSpeed + config::MotorSpeedInc) % config::MaxMotorSpeed);
    send(IFilamentCoilMotor::CommandSetMotorOffsetSpeed, Json({{protocol::IdSpeed, m_motorOffsetSpeed}}));
    return createResponse(command);
}

message::CommandResponse FilamentCoilControl::onCommandFilamentTensionSensorErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::ErrorOccurred));
}

message::CommandResponse FilamentCoilControl::onCommandFilamentCoilMotorStartMotorSucceeded(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotorSucceeded));
}

message::CommandResponse FilamentCoilControl::onCommandFilamentCoilMotorStartMotorFailed(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotorFailed));
}

message::CommandResponse FilamentCoilControl::onCommandFilamentCoilMotorErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::ErrorOccurred));
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
    m_motorOffsetSpeed = 0;
    const auto response =
        send(IFilamentCoilMotor::CommandSetMotorOffsetSpeed, Json({{protocol::IdSpeed, m_motorOffsetSpeed}}));
    if (response.result() == message::CommandResponse_Result_SUCCESS)
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
    (void)send(IFilamentCoilMotor::CommandSwitchOff);
    (void)send(IFilamentTensionSensor::CommandSwitchOff);
}

void FilamentCoilControl::handleError(const IFilamentCoilControl::Event& event,
                                      const IFilamentCoilControl::State& state)
{
    switchOff(event, state);
    notify(IFilamentCoilControl::NotificationErrorOccurred);
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
}

void FilamentCoilControl::startMotor(const IFilamentCoilControl::Event&,
                                    const IFilamentCoilControl::State&)
{
    const auto response = send(IFilamentCoilMotor::CommandStartMotor);
    if (response.result() == message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::StartMotorSucceeded));
    }
    else
    {
        push(Event(EventId::StartMotorFailed));
    }
}
