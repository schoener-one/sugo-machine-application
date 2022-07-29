
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

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentPreHeater::onCommandSwitchOn(const message::Command&)
{
    LOG(debug) << "command SwitchOn received in FilamentPreHeater...";
    return message::CommandResponse();
}

message::CommandResponse FilamentPreHeater::onCommandSwitchOff(const message::Command&)
{
    LOG(debug) << "command SwitchOff received in FilamentPreHeater...";
    return message::CommandResponse();
}

message::CommandResponse FilamentPreHeater::onCommandGetTemperature(const message::Command&)
{
    LOG(debug) << "command GetTemperature received in FilamentPreHeater...";
    return message::CommandResponse();
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentPreHeater::handleError(const IFilamentPreHeater::Event&,
                                    const IFilamentPreHeater::State&)
{
    LOG(debug) << "transition action handleError in FilamentPreHeater...";
}

void FilamentPreHeater::switchOn(const IFilamentPreHeater::Event&, const IFilamentPreHeater::State&)
{
    LOG(debug) << "transition action switchOn in FilamentPreHeater...";
}

void FilamentPreHeater::switchOff(const IFilamentPreHeater::Event&,
                                  const IFilamentPreHeater::State&)
{
    LOG(debug) << "transition action switchOff in FilamentPreHeater...";
}
