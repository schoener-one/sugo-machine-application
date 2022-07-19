
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-19
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentFeederMotor.hpp"

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentFeederMotor::onCommandSwitchOn(const message::Command&)
{
    LOG(debug) << "command SwitchOn received in FilamentFeederMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentFeederMotor::onCommandSwitchOff(const message::Command&)
{
    LOG(debug) << "command SwitchOff received in FilamentFeederMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentFeederMotor::onCommandGetState(const message::Command&)
{
    LOG(debug) << "command GetState received in FilamentFeederMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentFeederMotor::onCommandStartMotor(const message::Command&)
{
    LOG(debug) << "command StartMotor received in FilamentFeederMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentFeederMotor::onCommandStopMotor(const message::Command&)
{
    LOG(debug) << "command StopMotor received in FilamentFeederMotor...";
    return message::CommandResponse();
}

message::CommandResponse FilamentFeederMotor::onCommandSetMotorSpeed(const message::Command&)
{
    LOG(debug) << "command SetMotorSpeed received in FilamentFeederMotor...";
    return message::CommandResponse();
}


///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentFeederMotor::startMotor(const IFilamentFeederMotor::Event&, const IFilamentFeederMotor::State&)
{
    LOG(debug) << "transition action startMotor in FilamentFeederMotor...";
}

void FilamentFeederMotor::switchOn(const IFilamentFeederMotor::Event&, const IFilamentFeederMotor::State&)
{
    LOG(debug) << "transition action switchOn in FilamentFeederMotor...";
}

void FilamentFeederMotor::handleError(const IFilamentFeederMotor::Event&, const IFilamentFeederMotor::State&)
{
    LOG(debug) << "transition action handleError in FilamentFeederMotor...";
}

void FilamentFeederMotor::stopMotor(const IFilamentFeederMotor::Event&, const IFilamentFeederMotor::State&)
{
    LOG(debug) << "transition action stopMotor in FilamentFeederMotor...";
}

void FilamentFeederMotor::switchOff(const IFilamentFeederMotor::Event&, const IFilamentFeederMotor::State&)
{
    LOG(debug) << "transition action switchOff in FilamentFeederMotor...";
}

