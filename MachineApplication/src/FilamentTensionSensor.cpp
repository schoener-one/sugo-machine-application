
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

message::CommandResponse FilamentTensionSensor::onCommandSwitchOn(const message::Command&)
{
    LOG(debug) << "command SwitchOn received in FilamentTensionSensor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentTensionSensor::onCommandSwitchOff(const message::Command&)
{
    LOG(debug) << "command SwitchOff received in FilamentTensionSensor...";
    return message::CommandResponse();
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentTensionSensor::switchOff(const IFilamentTensionSensor::Event&,
                                      const IFilamentTensionSensor::State&)
{
    LOG(debug) << "transition action switchOff in FilamentTensionSensor...";
}

void FilamentTensionSensor::switchOn(const IFilamentTensionSensor::Event&,
                                     const IFilamentTensionSensor::State&)
{
    LOG(debug) << "transition action switchOn in FilamentTensionSensor...";
}

void FilamentTensionSensor::handleError(const IFilamentTensionSensor::Event&,
                                        const IFilamentTensionSensor::State&)
{
    LOG(debug) << "transition action handleError in FilamentTensionSensor...";
}
