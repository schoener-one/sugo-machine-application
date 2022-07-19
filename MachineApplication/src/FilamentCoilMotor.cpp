
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-19
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentCoilMotor.hpp"

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentCoilMotor::onCommandSwitchOn(const message::Command&)
{
    LOG(debug) << "command SwitchOn received in FilamentCoilMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilMotor::onCommandSwitchOff(const message::Command&)
{
    LOG(debug) << "command SwitchOff received in FilamentCoilMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilMotor::onCommandGetState(const message::Command&)
{
    LOG(debug) << "command GetState received in FilamentCoilMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilMotor::onCommandStartMotor(const message::Command&)
{
    LOG(debug) << "command StartMotor received in FilamentCoilMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilMotor::onCommandStopMotor(const message::Command&)
{
    LOG(debug) << "command StopMotor received in FilamentCoilMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilMotor::onCommandSetMotorSpeed(const message::Command&)
{
    LOG(debug) << "command SetMotorSpeed received in FilamentCoilMotor...";
    return message::CommandResponse();
}


///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentCoilMotor::startMotor(const IFilamentCoilMotor::Event&, const IFilamentCoilMotor::State&)
{
    LOG(debug) << "transition action startMotor in FilamentCoilMotor...";
}

void FilamentCoilMotor::switchOn(const IFilamentCoilMotor::Event&, const IFilamentCoilMotor::State&)
{
    LOG(debug) << "transition action switchOn in FilamentCoilMotor...";
}

void FilamentCoilMotor::handleError(const IFilamentCoilMotor::Event&, const IFilamentCoilMotor::State&)
{
    LOG(debug) << "transition action handleError in FilamentCoilMotor...";
}

void FilamentCoilMotor::stopMotor(const IFilamentCoilMotor::Event&, const IFilamentCoilMotor::State&)
{
    LOG(debug) << "transition action stopMotor in FilamentCoilMotor...";
}

void FilamentCoilMotor::switchOff(const IFilamentCoilMotor::Event&, const IFilamentCoilMotor::State&)
{
    LOG(debug) << "transition action switchOff in FilamentCoilMotor...";
}

