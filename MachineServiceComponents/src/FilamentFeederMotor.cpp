
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
using namespace sugo::message;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentFeederMotor::onCommandSwitchOn(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOn);
}

message::CommandResponse FilamentFeederMotor::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOff);
}

message::CommandResponse FilamentFeederMotor::onCommandStartMotor(const message::Command& command)
{
    return handleEventMessage(command, Event::StartMotor);
}

message::CommandResponse FilamentFeederMotor::onCommandStopMotor(const message::Command& command)
{
    return handleEventMessage(command, Event::StopMotor);
}

message::CommandResponse FilamentFeederMotor::onCommandSetMotorSpeed(
    const message::Command& command)
{
    const auto  parameters = Json::parse(command.parameters());
    const auto& motorSpeed = parameters.at(protocol::IdSpeed);

    if (motorSpeed.empty())
    {
        return createErrorCommandResponse(
            command, Json({{message::protocol::IdErrorReason,
                            message::protocol::IdErrorCommandParameterInvalid}}));
    }

    (void)setMotorSpeed(motorSpeed.get<unsigned>());
    return createCommandResponse(command);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentFeederMotor::switchOn(const IFilamentFeederMotor::Event&,
                                   const IFilamentFeederMotor::State&)
{
    if (resetMotor())
    {
        push(Event::SwitchOnSucceeded);
    }
    else
    {
        push(Event::SwitchOnFailed);
    }
}

void FilamentFeederMotor::startMotor(const IFilamentFeederMotor::Event&,
                                     const IFilamentFeederMotor::State&)
{
    if (startMotorRotation())
    {
        push(Event::StartMotorSucceeded);
        notify(NotificationStartMotorSucceeded);
    }
    else
    {
        push(Event::ErrorOccurred);
        notify(NotificationErrorOccurred);
    }
}

void FilamentFeederMotor::handleError(const IFilamentFeederMotor::Event&,
                                      const IFilamentFeederMotor::State&)
{
    stopMotorRotation(true);
    notify(NotificationErrorOccurred);
}

void FilamentFeederMotor::stopMotor(const IFilamentFeederMotor::Event&,
                                    const IFilamentFeederMotor::State&)
{
    stopMotorRotation(false);
    push(Event::StopMotorSucceeded);
    notify(NotificationStopMotorSucceeded);
}
