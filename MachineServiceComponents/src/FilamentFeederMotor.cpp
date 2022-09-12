
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentFeederMotor.hpp"
#include "HardwareAbstractionLayerHelper.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "MachineConfig.hpp"
#include "MachineProtocol.hpp"
#include "MessageProtocol.hpp"

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentFeederMotor::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentFeederMotor::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentFeederMotor::onCommandStartMotor(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotor));
}

message::CommandResponse FilamentFeederMotor::onCommandStopMotor(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StopMotor));
}

message::CommandResponse FilamentFeederMotor::onCommandSetMotorSpeed(
    const message::Command& command)
{
    const auto& motorSpeed = Json::parse(command.parameters()).at(protocol::IdSpeed);
    if (motorSpeed.empty())
    {
        return createErrorResponse(
            command, Json({{protocol::IdErrorReason, protocol::IdErrorCommandParameterInvalid}}));
    }
    setMotorSpeed(motorSpeed.get<unsigned>());
    return createResponse(command);
}

message::CommandResponse FilamentFeederMotor::onCommandSetMotorOffsetSpeed(
    const message::Command& command)
{
    return createUnsupportedCommandResponse(command);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentFeederMotor::switchOn(const IFilamentFeederMotor::Event&,
                                   const IFilamentFeederMotor::State&)
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

void FilamentFeederMotor::startMotor(const IFilamentFeederMotor::Event&,
                                     const IFilamentFeederMotor::State&)
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

void FilamentFeederMotor::handleError(const IFilamentFeederMotor::Event&,
                                      const IFilamentFeederMotor::State&)
{
    stopMotorRotation(true);
    notify(NotificationErrorOccurred);
}

void FilamentFeederMotor::switchOff(const IFilamentFeederMotor::Event&,
                                    const IFilamentFeederMotor::State&)
{
    // Nothing to do!
}

void FilamentFeederMotor::stopMotor(const IFilamentFeederMotor::Event&,
                                    const IFilamentFeederMotor::State&)
{
    stopMotorRotation(false);
}
