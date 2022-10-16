
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentCoilMotor.hpp"
#include "HardwareAbstractionLayerHelper.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "MachineConfig.hpp"
#include "MachineProtocol.hpp"
#include "MessageProtocol.hpp"

using namespace sugo;
using namespace sugo::message::protocol;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentCoilMotor::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentCoilMotor::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentCoilMotor::onCommandStartMotor(const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::StartMotor));
}

message::CommandResponse FilamentCoilMotor::onCommandStopMotor(const message::Command& command)
{
    return handleStateChangeMessage(command, Event(EventId::StopMotor));
}

message::CommandResponse FilamentCoilMotor::onCommandSetMotorSpeed(const message::Command& command)
{
    const auto& motorSpeed = Json::parse(command.parameters()).at(protocol::IdSpeed);
    if (motorSpeed.empty())
    {
        return createErrorCommandResponse(
            command, Json({{message::protocol::IdErrorReason,
                            message::protocol::IdErrorCommandParameterInvalid}}));
    }
    setMotorSpeed(motorSpeed.get<unsigned>());
    return createCommandResponse(command);
}

message::CommandResponse FilamentCoilMotor::onCommandSetMotorOffsetSpeed(
    const message::Command& command)
{
    const auto& motorSpeed = Json::parse(command.parameters()).at(protocol::IdSpeed);
    if (motorSpeed.empty())
    {
        return createErrorCommandResponse(
            command, Json({{message::protocol::IdErrorReason,
                            message::protocol::IdErrorCommandParameterInvalid}}));
    }
    setMotorOffsetSpeed(motorSpeed.get<unsigned>());
    return createCommandResponse(command);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentCoilMotor::switchOn(const IFilamentCoilMotor::Event&, const IFilamentCoilMotor::State&)
{
    if (resetMotor())
    {
        push(Event(EventId::SwitchOnSucceeded));
    }
    else
    {
        push(Event(EventId::SwitchOnFailed));
    }
}

void FilamentCoilMotor::startMotor(const IFilamentCoilMotor::Event&,
                                   const IFilamentCoilMotor::State&)
{
    if (startMotorRotation())
    {
        push(Event(EventId::StartMotorSucceeded));
        notify(NotificationStartMotorSucceeded);
    }
    else
    {
        push(Event(EventId::StartMotorFailed));
        notify(NotificationStartMotorFailed);
    }
}

void FilamentCoilMotor::handleError(const IFilamentCoilMotor::Event&,
                                    const IFilamentCoilMotor::State&)
{
    stopMotorRotation(true);
    notify(NotificationErrorOccurred);
}

void FilamentCoilMotor::stopMotor(const IFilamentCoilMotor::Event&,
                                  const IFilamentCoilMotor::State&)
{
    stopMotorRotation(false);
}
