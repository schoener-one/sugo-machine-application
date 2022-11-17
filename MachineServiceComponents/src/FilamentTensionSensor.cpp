
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentTensionSensor.hpp"
#include "Identifier.hpp"

using namespace sugo;
using namespace sugo::message;

FilamentTensionSensor::FilamentTensionSensor(message::ICommandMessageBroker& messageBroker,
                                             const ServiceLocator&           serviceLocator)
    : IFilamentTensionSensor(messageBroker),
      FilamentTensionControlService(hal::id::GpioPinSignalFilamentTensionLow,
                                    hal::id::GpioPinSignalFilamentTensionHigh, serviceLocator),
      m_serviceLocator(serviceLocator)
{
}

void FilamentTensionSensor::onFilamentTensionEvent(FilamentTensionEvent event)
{
    if (event == FilamentTensionLow)
    {
        notify(IFilamentTensionSensor::NotificationTensionTooLow);
    }
    else if (event == FilamentTensionHigh)
    {
        notify(IFilamentTensionSensor::NotificationTensionTooHigh);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentTensionSensor::onCommandSwitchOn(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOn);
}

message::CommandResponse FilamentTensionSensor::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOff);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentTensionSensor::switchOff(const IFilamentTensionSensor::Event&,
                                      const IFilamentTensionSensor::State&)
{
    stopSensorObservation();
}

void FilamentTensionSensor::switchOn(const IFilamentTensionSensor::Event&,
                                     const IFilamentTensionSensor::State&)
{
    if (!startSensorObservation())
    {
        push(Event::SwitchOnFailed);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void FilamentTensionSensor::handleError(const IFilamentTensionSensor::Event&,
                                        const IFilamentTensionSensor::State&)
{
    stopSensorObservation();
    notify(NotificationErrorOccurred);
}
