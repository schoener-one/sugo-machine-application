
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentPreHeater.hpp"
#include "MachineProtocol.hpp"
#include "MachineConfig.hpp"

using namespace sugo;

void FilamentPreHeater::onMaxTemperatureReached()
{
    push(Event(EventId::MaxTemperatureReached));
    processAllEvents();
}

void FilamentPreHeater::onMinTemperatureReached()
{
    push(Event(EventId::MinTemperatureReached));
    processAllEvents();
}

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentPreHeater::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentPreHeater::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentPreHeater::onCommandGetTemperature(const message::Command& command)
{
    return createResponse(command, Json({{protocol::IdTemperature, getTemperature()}}));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentPreHeater::switchOn(const IFilamentPreHeater::Event&, const IFilamentPreHeater::State&)
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

void FilamentPreHeater::startHeating(const IFilamentPreHeater::Event&,
                                     const IFilamentPreHeater::State&)
{
    if (!switchHeater(config::GpioPinRelaySwitchFeederHeaterId, true))
    {
        push(Event(EventId::ErrorOccurred));
    }
}

void FilamentPreHeater::stopHeating(const IFilamentPreHeater::Event&,
                                    const IFilamentPreHeater::State&)
{
    if (!m_hasNotifiedTargetTemperatureReached)
    {
        notify(NotificationTargetTemperatureReached);
        m_hasNotifiedTargetTemperatureReached = true;
    }
    if (!switchHeater(config::GpioPinRelaySwitchFeederHeaterId, false))
    {
        push(Event(EventId::ErrorOccurred));
    }
}

void FilamentPreHeater::switchOff(const IFilamentPreHeater::Event&,
                                  const IFilamentPreHeater::State&)
{
    stopTemperatureObservation();
    if (!switchHeater(config::GpioPinRelaySwitchFeederHeaterId, false))
    {
        push(Event(EventId::ErrorOccurred));
    }
}

void FilamentPreHeater::handleError(const IFilamentPreHeater::Event&,
                                    const IFilamentPreHeater::State&)
{
    (void)switchHeater(config::GpioPinRelaySwitchFeederHeaterId, false);
    notify(NotificationErrorOccurred);
}
