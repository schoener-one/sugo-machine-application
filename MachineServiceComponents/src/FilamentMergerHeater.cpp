
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
using namespace sugo::message;

FilamentMergerHeater::FilamentMergerHeater(ICommandMessageBroker& messageBroker,
                                           const ServiceLocator&  serviceLocator)
    : IFilamentMergerHeater(messageBroker),
      HeaterService(config::GpioPinRelaySwitchMergerHeaterId, config::TemperatureSensorMergerId,
                    serviceLocator)
{
}

void FilamentMergerHeater::onTemperatureLimitEvent(TemperatureLimitEvent event)
{
    switch (event)
    {
        case MinTemperatureReached:
            push(Event(EventId::MinTemperatureReached));
            break;
        case MaxTemperatureReached:
            push(Event(EventId::MaxTemperatureReached));
            break;
    }

    processAllEvents();
}

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentMergerHeater::onCommandSwitchOn(const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentMergerHeater::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentMergerHeater::onCommandGetTemperature(
    const message::Command& command)
{
    return createCommandResponse(command, Json({{protocol::IdTemperature, getTemperature()}}));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentMergerHeater::switchOn(const IFilamentMergerHeater::Event&,
                                    const IFilamentMergerHeater::State&)
{
    updateHeaterTemperature();
    if (!startTemperatureObservation())
    {
        push(Event(EventId::SwitchOnFailed));
        return;
    }
    push(Event(EventId::SwitchOnSucceeded));
}

void FilamentMergerHeater::startHeating(const IFilamentMergerHeater::Event& event,
                                        const IFilamentMergerHeater::State&)
{
    if (!switchHeater(true))
    {
        push(Event(EventId::ErrorOccurred));
    }
    if (event.getId() == EventId::MinTemperatureReached)
    {
        notify(NotificationTargetTemperatureRangeLeft);
    }
}

void FilamentMergerHeater::stopHeating(const IFilamentMergerHeater::Event& event,
                                       const IFilamentMergerHeater::State&)
{
    if (!switchHeater(false))
    {
        push(Event(EventId::ErrorOccurred));
    }
    if (event.getId() == EventId::MaxTemperatureReached)
    {
        notify(NotificationTargetTemperatureRangeReached);
    }
}

void FilamentMergerHeater::switchOff(const IFilamentMergerHeater::Event&,
                                     const IFilamentMergerHeater::State&)
{
    stopTemperatureObservation();
    if (!switchHeater(false))
    {
        push(Event(EventId::ErrorOccurred));
    }
}

void FilamentMergerHeater::handleError(const IFilamentMergerHeater::Event&,
                                       const IFilamentMergerHeater::State&)
{
    (void)switchHeater(false);
    notify(NotificationErrorOccurred);
}
