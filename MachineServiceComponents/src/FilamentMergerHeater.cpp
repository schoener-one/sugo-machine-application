
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentMergerHeater.hpp"
#include "MachineConfig.hpp"
#include "MachineProtocol.hpp"

using namespace sugo;

void FilamentMergerHeater::onMaxTemperatureReached()
{
    push(Event(EventId::MaxTemperatureReached));
    processAllEvents();
}

void FilamentMergerHeater::onMinTemperatureReached()
{
    push(Event(EventId::MinTemperatureReached));
    processAllEvents();
}

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentMergerHeater::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentMergerHeater::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentMergerHeater::onCommandGetTemperature(
    const message::Command& command)
{
    return createResponse(command, Json({{protocol::IdTemperature, getTemperature()}}));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentMergerHeater::switchOn(const IFilamentMergerHeater::Event&,
                                    const IFilamentMergerHeater::State&)
{
    m_hasNotifiedTargetTemperatureReached = false;
    updateHeaterTemperature();
    if (!startTemperatureObservation())
    {
        push(Event(EventId::SwitchOnFailed));
        return;
    }
    push(Event(EventId::SwitchOnSucceeded));
}

void FilamentMergerHeater::startHeating(const IFilamentMergerHeater::Event&,
                                        const IFilamentMergerHeater::State&)
{
    if (!switchHeater(config::GpioPinRelaySwitchMergerHeaterId, true))
    {
        push(Event(EventId::ErrorOccurred));
    }
}

void FilamentMergerHeater::stopHeating(const IFilamentMergerHeater::Event&,
                                       const IFilamentMergerHeater::State&)
{
    if (!m_hasNotifiedTargetTemperatureReached)
    {
        notify(NotificationTargetTemperatureReached);
        m_hasNotifiedTargetTemperatureReached = true;
    }
    if (!switchHeater(config::GpioPinRelaySwitchMergerHeaterId, false))
    {
        push(Event(EventId::ErrorOccurred));
    }
}

void FilamentMergerHeater::switchOff(const IFilamentMergerHeater::Event&,
                                     const IFilamentMergerHeater::State&)
{
    stopTemperatureObservation();
    if (!switchHeater(config::GpioPinRelaySwitchMergerHeaterId, false))
    {
        push(Event(EventId::ErrorOccurred));
    }
}

void FilamentMergerHeater::handleError(const IFilamentMergerHeater::Event&,
                                       const IFilamentMergerHeater::State&)
{
    (void)switchHeater(config::GpioPinRelaySwitchMergerHeaterId, false);
    notify(NotificationErrorOccurred);
}
