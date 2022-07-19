
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-19
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentMergerControl.hpp"

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentMergerControl::onCommandSwitchOn(const message::Command&)
{
    LOG(debug) << "command SwitchOn received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandSwitchOff(const message::Command&)
{
    LOG(debug) << "command SwitchOff received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandStartFeeding(const message::Command&)
{
    LOG(debug) << "command StartFeeding received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandStopFeeding(const message::Command&)
{
    LOG(debug) << "command StopFeeding received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandGetState(const message::Command&)
{
    LOG(debug) << "command GetState received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandGetTemperatures(const message::Command&)
{
    LOG(debug) << "command GetTemperatures received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandIncreaseMotorSpeed(const message::Command&)
{
    LOG(debug) << "command IncreaseMotorSpeed received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandDecreaseMotorSpeed(const message::Command&)
{
    LOG(debug) << "command DecreaseMotorSpeed received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentPreHeaterTargetTemperatureReached(const message::Command&)
{
    LOG(debug) << "command FilamentPreHeater.TargetTemperatureReached received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentPreHeaterHeatingStopped(const message::Command&)
{
    LOG(debug) << "command FilamentPreHeater.HeatingStopped received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentPreHeaterErrorOccurred(const message::Command&)
{
    LOG(debug) << "command FilamentPreHeater.ErrorOccurred received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentMergerHeaterTargetTemperatureReached(const message::Command&)
{
    LOG(debug) << "command FilamentMergerHeater.TargetTemperatureReached received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentMergerHeaterHeatingStopped(const message::Command&)
{
    LOG(debug) << "command FilamentMergerHeater.HeatingStopped received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentMergerHeaterErrorOccurred(const message::Command&)
{
    LOG(debug) << "command FilamentMergerHeater.ErrorOccurred received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentFeederMotorStartMotorSucceeded(const message::Command&)
{
    LOG(debug) << "command FilamentFeederMotor.StartMotorSucceeded received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentFeederMotorStartMotorFailed(const message::Command&)
{
    LOG(debug) << "command FilamentFeederMotor.StartMotorFailed received in FilamentMergerControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentMergerControl::onCommandFilamentFeederMotorErrorOccurred(const message::Command&)
{
    LOG(debug) << "command FilamentFeederMotor.ErrorOccurred received in FilamentMergerControl...";
    return message::CommandResponse();
}


///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentMergerControl::startMotor(const IFilamentMergerControl::Event&, const IFilamentMergerControl::State&)
{
    LOG(debug) << "transition action startMotor in FilamentMergerControl...";
}

void FilamentMergerControl::switchOn(const IFilamentMergerControl::Event&, const IFilamentMergerControl::State&)
{
    LOG(debug) << "transition action switchOn in FilamentMergerControl...";
}

void FilamentMergerControl::heatingUp(const IFilamentMergerControl::Event&, const IFilamentMergerControl::State&)
{
    LOG(debug) << "transition action heatingUp in FilamentMergerControl...";
}

void FilamentMergerControl::handleError(const IFilamentMergerControl::Event&, const IFilamentMergerControl::State&)
{
    LOG(debug) << "transition action handleError in FilamentMergerControl...";
}

void FilamentMergerControl::stopMotor(const IFilamentMergerControl::Event&, const IFilamentMergerControl::State&)
{
    LOG(debug) << "transition action stopMotor in FilamentMergerControl...";
}

void FilamentMergerControl::switchOff(const IFilamentMergerControl::Event&, const IFilamentMergerControl::State&)
{
    LOG(debug) << "transition action switchOff in FilamentMergerControl...";
}

