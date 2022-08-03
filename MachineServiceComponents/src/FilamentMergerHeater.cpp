
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

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentMergerHeater::onCommandSwitchOn(const message::Command&)
{
    LOG(debug) << "command SwitchOn received in FilamentMergerHeater...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerHeater::onCommandSwitchOff(const message::Command&)
{
    LOG(debug) << "command SwitchOff received in FilamentMergerHeater...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerHeater::onCommandGetTemperature(const message::Command&)
{
    LOG(debug) << "command GetTemperature received in FilamentMergerHeater...";
    return message::CommandResponse();
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentMergerHeater::handleError(const IFilamentMergerHeater::Event&,
                                       const IFilamentMergerHeater::State&)
{
    LOG(debug) << "transition action handleError in FilamentMergerHeater...";
}

void FilamentMergerHeater::switchOn(const IFilamentMergerHeater::Event&,
                                    const IFilamentMergerHeater::State&)
{
    LOG(debug) << "transition action switchOn in FilamentMergerHeater...";
}

void FilamentMergerHeater::switchOff(const IFilamentMergerHeater::Event&,
                                     const IFilamentMergerHeater::State&)
{
    LOG(debug) << "transition action switchOff in FilamentMergerHeater...";
}
