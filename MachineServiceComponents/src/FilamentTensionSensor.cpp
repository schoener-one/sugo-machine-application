
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

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentTensionSensor::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentTensionSensor::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentTensionSensor::switchOff(const IFilamentTensionSensor::Event&,
                                      const IFilamentTensionSensor::State&)
{
    // Nothing to do!
}

void FilamentTensionSensor::switchOn(const IFilamentTensionSensor::Event&,
                                     const IFilamentTensionSensor::State&)
{
    push(Event(EventId::SwitchOnSucceeded));
}

void FilamentTensionSensor::handleError(const IFilamentTensionSensor::Event&,
                                        const IFilamentTensionSensor::State&)
{
    notify(NotificationErrorOccurred);
}
