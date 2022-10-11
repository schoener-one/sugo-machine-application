
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentMergerControl.hpp"
#include "IFilamentFeederMotor.hpp"
#include "IFilamentMergerHeater.hpp"
#include "IFilamentPreHeater.hpp"

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentMergerControl::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentMergerControl::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentMergerControl::onCommandStartFeeding(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotor));
}

message::CommandResponse FilamentMergerControl::onCommandStopFeeding(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StopMotor));
}

message::CommandResponse FilamentMergerControl::onCommandGetTemperatures(
    const message::Command& command)
{
    return forward(IFilamentMergerHeater::CommandGetTemperature, command);
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentPreHeaterTargetTemperatureRangeReached(
    const message::Command& command)
{
    const bool stateChange = !m_isPreHeaterTemperatureReached;
    m_isPreHeaterTemperatureReached = true;
    // TODO add support for state change conditions!
    if (stateChange && m_isMergerHeaterTemperatureReached && m_isPreHeaterTemperatureReached)
    {
        return handleStateChangeCommand(command, Event(EventId::HeatingUpSucceeded));
    }
    return createResponse(command);
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentPreHeaterErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::ErrorOccurred));
}

message::CommandResponse
FilamentMergerControl::onNotificationFilamentMergerHeaterTargetTemperatureRangeReached(
    const message::Command& command)
{
    const bool stateChange = !m_isMergerHeaterTemperatureReached;
    m_isMergerHeaterTemperatureReached = true;
    // TODO add support for state change conditions!
    if (stateChange && m_isMergerHeaterTemperatureReached && m_isPreHeaterTemperatureReached)
    {
        return handleStateChangeCommand(command, Event(EventId::HeatingUpSucceeded));
    }
    return createResponse(command);
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentMergerHeaterErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::ErrorOccurred));
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentFeederMotorStartMotorSucceeded(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotorSucceeded));
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentFeederMotorStartMotorFailed(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotorFailed));
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentFeederMotorErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::ErrorOccurred));
}

message::CommandResponse FilamentMergerControl::onCommandSetMotorSpeed(
    const message::Command& command)
{
    return forward(IFilamentFeederMotor::CommandSetMotorSpeed, command);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentMergerControl::switchOn(const IFilamentMergerControl::Event&,
                                     const IFilamentMergerControl::State&)
{
    m_isMergerHeaterTemperatureReached = false;
    m_isPreHeaterTemperatureReached    = false;
    const auto responseFeederMotor     = send(IFilamentFeederMotor::CommandSwitchOn);
    if (responseFeederMotor.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::SwitchOnFailed));
    }
    else
    {
        push(Event(EventId::SwitchOnSucceeded));
    }
}

void FilamentMergerControl::switchOff(const IFilamentMergerControl::Event&,
                                      const IFilamentMergerControl::State&)
{
    (void)send(IFilamentFeederMotor::CommandSwitchOff);
    (void)send(IFilamentMergerHeater::CommandSwitchOff);
    (void)send(IFilamentPreHeater::CommandSwitchOff);
}

void FilamentMergerControl::startMotor(const IFilamentMergerControl::Event&,
                                       const IFilamentMergerControl::State&)
{
    const auto response = send(IFilamentFeederMotor::CommandStartMotor);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::StartMotorFailed));
    }
}

void FilamentMergerControl::stopMotor(const IFilamentMergerControl::Event&,
                                      const IFilamentMergerControl::State&)
{
    const auto response = send(IFilamentFeederMotor::CommandStopMotor);
    if (response.result() == message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::StopMotorSucceeded));
    }
    else
    {
        push(Event(EventId::StopMotorFailed));
    }
    notify(NotificationFeedingStopped);
}

void FilamentMergerControl::handleError(const IFilamentMergerControl::Event& event,
                                        const IFilamentMergerControl::State& state)
{
    switchOff(event, state);
    notify(NotificationErrorOccurred);
}

void FilamentMergerControl::notifyHeatedUp(const Event&, const State&)
{
    notify(NotificationHeatedUp);
}

void FilamentMergerControl::heatingUp(const IFilamentMergerControl::Event&,
                                      const IFilamentMergerControl::State&)
{
    const auto responsePreHeater = send(IFilamentPreHeater::CommandSwitchOn);
    if (responsePreHeater.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::HeatingUpFailed));
    }
    const auto responseMergerHeater = send(IFilamentMergerHeater::CommandSwitchOn);
    if (responseMergerHeater.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::HeatingUpFailed));
    }
}

void FilamentMergerControl::notifyRunning(const IFilamentMergerControl::Event&,
                                        const IFilamentMergerControl::State&)
{
    notify(NotificationFeedingRunning);
}