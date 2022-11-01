
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
#include "MachineConfig.hpp"

using namespace sugo;
using namespace sugo::message;

FilamentTensionSensor::FilamentTensionSensor(message::ICommandMessageBroker& messageBroker,
                                             const ServiceLocator&           serviceLocator)
    : IFilamentTensionSensor(messageBroker),
      FilamentTensionControlService(config::GpioPinSignalFilamentTensionLowId,
                                    config::GpioPinSignalFilamentTensionHighId, serviceLocator),
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
    return handleEventMessage(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentTensionSensor::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event(EventId::SwitchOff));
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
        push(Event(EventId::SwitchOnFailed));
        return;
    }

    push(Event(EventId::SwitchOnSucceeded));
}

void FilamentTensionSensor::handleError(const IFilamentTensionSensor::Event&,
                                        const IFilamentTensionSensor::State&)
{
    stopSensorObservation();
    notify(NotificationErrorOccurred);
}
