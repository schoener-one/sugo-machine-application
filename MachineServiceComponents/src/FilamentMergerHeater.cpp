
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
#include "Identifier.hpp"
#include "MachineProtocol.hpp"

using namespace sugo;
using namespace sugo::message;

FilamentMergerHeater::FilamentMergerHeater(ICommandMessageBroker& messageBroker,
                                           const ServiceLocator&  serviceLocator)
    : IFilamentMergerHeater(messageBroker),
      HeaterService(hal::id::GpioPinRelaySwitchHeaterMerger, hal::id::TemperatureSensorMerger,
                    serviceLocator)
{
}

void FilamentMergerHeater::onTemperatureLimitEvent(TemperatureLimitEvent event)
{
    switch (event)
    {
        case MinTemperatureReached:
            push(Event::MinTemperatureReached);
            break;
        case MaxTemperatureReached:
            push(Event::MaxTemperatureReached);
            break;
    }

    processAllEvents();
}

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentMergerHeater::onCommandSwitchOn(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOn);
}

message::CommandResponse FilamentMergerHeater::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOff);
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
        push(Event::SwitchOnFailed);
        return;
    }
    push(Event::SwitchOnSucceeded);
}

void FilamentMergerHeater::startHeating(const IFilamentMergerHeater::Event& event,
                                        const IFilamentMergerHeater::State&)
{
    if (!switchHeater(true))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (event == Event::MinTemperatureReached)
    {
        notify(NotificationTargetTemperatureRangeLeft);
    }
}

void FilamentMergerHeater::stopHeating(const IFilamentMergerHeater::Event& event,
                                       const IFilamentMergerHeater::State&)
{
    if (!switchHeater(false))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (event == Event::MaxTemperatureReached)
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
        push(Event::ErrorOccurred);
    }
}

void FilamentMergerHeater::handleError(const IFilamentMergerHeater::Event&,
                                       const IFilamentMergerHeater::State&)
{
    (void)switchHeater(false);
    notify(NotificationErrorOccurred);
}
