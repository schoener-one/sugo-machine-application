
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
#include "MachineConfig.hpp"
#include "MachineProtocol.hpp"

using namespace sugo;
using namespace sugo::message;

FilamentPreHeater::FilamentPreHeater(ICommandMessageBroker& messageBroker,
                                     const ServiceLocator&  serviceLocator)
    : IFilamentPreHeater(messageBroker),
      HeaterService(config::GpioPinRelaySwitchFeederHeaterId, config::TemperatureSensorFeederId,
                    serviceLocator)
{
}

void FilamentPreHeater::onTemperatureLimitEvent(TemperatureLimitEvent event)
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

message::CommandResponse FilamentPreHeater::onCommandSwitchOn(const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentPreHeater::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentPreHeater::onCommandGetTemperature(const message::Command& command)
{
    return createCommandResponse(command, Json({{protocol::IdTemperature, getTemperature()}}));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentPreHeater::switchOn(const IFilamentPreHeater::Event&, const IFilamentPreHeater::State&)
{
    updateHeaterTemperature();
    
    if (!startTemperatureObservation())
    {
        push(Event(EventId::SwitchOnFailed));
        return;
    }

    push(Event(EventId::SwitchOnSucceeded));
}

void FilamentPreHeater::startHeating(const IFilamentPreHeater::Event& event,
                                     const IFilamentPreHeater::State&)
{
    if (!switchHeater(true))
    {
        push(Event(EventId::ErrorOccurred));
        return;
    }

    if (event.getId() == EventId::MinTemperatureReached)
    {
        notify(NotificationTargetTemperatureRangeLeft);
    }
}

void FilamentPreHeater::stopHeating(const IFilamentPreHeater::Event& event,
                                    const IFilamentPreHeater::State&)
{
    if (!switchHeater(false))
    {
        push(Event(EventId::ErrorOccurred));
        return;
    }

    if (event.getId() == EventId::MaxTemperatureReached)
    {
        notify(NotificationTargetTemperatureRangeReached);
    }
}

void FilamentPreHeater::switchOff(const IFilamentPreHeater::Event&,
                                  const IFilamentPreHeater::State&)
{
    stopTemperatureObservation();

    if (!switchHeater(false))
    {
        push(Event(EventId::ErrorOccurred));
    }
}

void FilamentPreHeater::handleError(const IFilamentPreHeater::Event&,
                                    const IFilamentPreHeater::State&)
{
    (void)switchHeater(false);
    notify(NotificationErrorOccurred);
}
