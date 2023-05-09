
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
using namespace sugo::message;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentMergerControl::onCommandSwitchOn(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOn);
}

message::CommandResponse FilamentMergerControl::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOff);
}

message::CommandResponse FilamentMergerControl::onCommandStartFeeding(
    const message::Command& command)
{
    return handleEventMessage(command, Event::StartMotor);
}

message::CommandResponse FilamentMergerControl::onCommandStopFeeding(
    const message::Command& command)
{
    return handleEventMessage(command, Event::Stop);
}

message::CommandResponse FilamentMergerControl::onCommandGetTemperatures(
    const message::Command& command)
{
    return forward(IFilamentMergerHeater::CommandGetTemperature, command);
}

message::CommandResponse FilamentMergerControl::onCommandStartHeating(
    const message::Command& command)
{
    return handleEventMessage(command, Event::StartHeating);
}

message::CommandResponse
FilamentMergerControl::onNotificationFilamentPreHeaterTargetTemperatureRangeReached(
    const message::Command& command)
{
    const bool stateChange          = !m_isPreHeaterTemperatureReached;
    m_isPreHeaterTemperatureReached = true;
    // TODO add support for state change conditions!
    if (stateChange && m_isMergerHeaterTemperatureReached && m_isPreHeaterTemperatureReached)
    {
        return handleEventMessage(command, Event::HeatingUpSucceeded);
    }
    return createCommandResponse(command);
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentPreHeaterErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event::ErrorOccurred);
}

message::CommandResponse
FilamentMergerControl::onNotificationFilamentMergerHeaterTargetTemperatureRangeReached(
    const message::Command& command)
{
    const bool stateChange             = !m_isMergerHeaterTemperatureReached;
    m_isMergerHeaterTemperatureReached = true;
    // TODO add support for state change conditions!
    if (stateChange && m_isMergerHeaterTemperatureReached && m_isPreHeaterTemperatureReached)
    {
        return handleEventMessage(command, Event::HeatingUpSucceeded);
    }
    return createCommandResponse(command);
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentMergerHeaterErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event::ErrorOccurred);
}

message::CommandResponse
FilamentMergerControl::onNotificationFilamentFeederMotorStartMotorSucceeded(
    const message::Command& command)
{
    return handleEventMessage(command, Event::StartMotorSucceeded);
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentFeederMotorStartMotorFailed(
    const message::Command& command)
{
    return handleEventMessage(command, Event::StartMotorFailed);
}

message::CommandResponse FilamentMergerControl::onNotificationFilamentFeederMotorErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event::ErrorOccurred);
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
        push(Event::SwitchOnFailed);
    }
    else
    {
        push(Event::SwitchOnSucceeded);
    }
}

void FilamentMergerControl::switchOff(const IFilamentMergerControl::Event&,
                                      const IFilamentMergerControl::State&)
{
    switchOff();
}

void FilamentMergerControl::switchOff()
{
    switchOffServiceComponent<IFilamentFeederMotor>();
    switchOffServiceComponent<IFilamentMergerHeater>();
    switchOffServiceComponent<IFilamentPreHeater>();
}

void FilamentMergerControl::startMotor(const IFilamentMergerControl::Event&,
                                       const IFilamentMergerControl::State&)
{
    const auto response = send(IFilamentFeederMotor::CommandStartMotor);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::StartMotorFailed);
    }
}

void FilamentMergerControl::stopMerger(const IFilamentMergerControl::Event&,
                                       const IFilamentMergerControl::State&)
{
    switchOffServiceComponent<IFilamentMergerHeater>();
    switchOffServiceComponent<IFilamentPreHeater>();

    const auto response = send(IFilamentFeederMotor::CommandStopMotor);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::ErrorOccurred);
    }
    notify(NotificationFeedingStopped);
}

void FilamentMergerControl::handleError(const IFilamentMergerControl::Event&,
                                        const IFilamentMergerControl::State&)
{
    switchOff();
    notify(NotificationErrorOccurred);
}

void FilamentMergerControl::notifyHeatedUp(const Event&, const State&)
{
    notify(NotificationHeatedUp);
}

void FilamentMergerControl::heatingUp(const IFilamentMergerControl::Event&,
                                      const IFilamentMergerControl::State&)
{
    m_isMergerHeaterTemperatureReached = false;
    m_isPreHeaterTemperatureReached    = false;
    const auto responsePreHeater       = send(IFilamentPreHeater::CommandSwitchOn);

    if (responsePreHeater.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::HeatingUpFailed);
    }

    const auto responseMergerHeater = send(IFilamentMergerHeater::CommandSwitchOn);

    if (responseMergerHeater.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::HeatingUpFailed);
    }
}

void FilamentMergerControl::notifyRunning(const IFilamentMergerControl::Event&,
                                          const IFilamentMergerControl::State&)
{
    notify(NotificationFeedingRunning);
}