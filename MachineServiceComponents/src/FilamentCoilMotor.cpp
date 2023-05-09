
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
    return handleEventMessage(command, Event::SwitchOn);
}

message::CommandResponse FilamentCoilMotor::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOff);
}

message::CommandResponse FilamentCoilMotor::onCommandStartMotor(const message::Command& command)
{
    return handleEventMessage(command, Event::StartMotor);
}

message::CommandResponse FilamentCoilMotor::onCommandStopMotor(const message::Command& command)
{
    return handleEventMessage(command, Event::StopMotor);
}

message::CommandResponse FilamentCoilMotor::onCommandSetMotorSpeed(const message::Command& command)
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

message::CommandResponse FilamentCoilMotor::onCommandIncreaseMotorOffsetSpeed(
    const message::Command& command)
{
    const int motorSpeedIncrement = static_cast<int>(
        m_serviceLocator.get<IConfiguration>().getOption(id::MotorSpeedIncrement).get<unsigned>());
    (void)addMotorOffsetSpeed(motorSpeedIncrement);
    return createCommandResponse(command);
}

message::CommandResponse FilamentCoilMotor::onCommandDecreaseMotorOffsetSpeed(
    const message::Command& command)
{
    static constexpr int negativeMultiplier  = -1;
    const int            motorSpeedIncrement = static_cast<int>(
        m_serviceLocator.get<IConfiguration>().getOption(id::MotorSpeedIncrement).get<unsigned>());
    (void)addMotorOffsetSpeed(negativeMultiplier * motorSpeedIncrement);
    return createCommandResponse(command);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentCoilMotor::switchOn(const IFilamentCoilMotor::Event&, const IFilamentCoilMotor::State&)
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

void FilamentCoilMotor::startMotor(const IFilamentCoilMotor::Event&,
                                   const IFilamentCoilMotor::State&)
{
    if (startMotorRotation())
    {
        push(Event::StartMotorSucceeded);
        notify(NotificationStartMotorSucceeded);
    }
    else
    {
        push(Event::StartMotorFailed);
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
