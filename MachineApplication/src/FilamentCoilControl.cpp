
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-19
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentCoilControl.hpp"

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentCoilControl::onCommandSwitchOn(const message::Command&)
{
    LOG(debug) << "command SwitchOn received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandSwitchOff(const message::Command&)
{
    LOG(debug) << "command SwitchOff received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandStartCoil(const message::Command&)
{
    LOG(debug) << "command StartCoil received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandStopCoil(const message::Command&)
{
    LOG(debug) << "command StopCoil received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandGetState(const message::Command&)
{
    LOG(debug) << "command GetState received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandIncreaseMotorSpeed(const message::Command&)
{
    LOG(debug) << "command IncreaseMotorSpeed received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandDecreaseMotorSpeed(const message::Command&)
{
    LOG(debug) << "command DecreaseMotorSpeed received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandFilamentTensionSensorTensionTooLow(const message::Command&)
{
    LOG(debug) << "command FilamentTensionSensor.TensionTooLow received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandFilamentTensionSensorTensionTooHigh(const message::Command&)
{
    LOG(debug) << "command FilamentTensionSensor.TensionTooHigh received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandFilamentTensionSensorErrorOccurred(const message::Command&)
{
    LOG(debug) << "command FilamentTensionSensor.ErrorOccurred received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandFilamentCoilMotorStartMotorSucceeded(const message::Command&)
{
    LOG(debug) << "command FilamentCoilMotor.StartMotorSucceeded received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandFilamentCoilMotorStartMotorFailed(const message::Command&)
{
    LOG(debug) << "command FilamentCoilMotor.StartMotorFailed received in FilamentCoilControl...";
    return message::CommandResponse();
}

message::CommandResponse FilamentCoilControl::onCommandFilamentCoilMotorErrorOccurred(const message::Command&)
{
    LOG(debug) << "command FilamentCoilMotor.ErrorOccurred received in FilamentCoilControl...";
    return message::CommandResponse();
}


///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentCoilControl::stopCoil(const IFilamentCoilControl::Event&, const IFilamentCoilControl::State&)
{
    LOG(debug) << "transition action stopCoil in FilamentCoilControl...";
}

void FilamentCoilControl::switchOn(const IFilamentCoilControl::Event&, const IFilamentCoilControl::State&)
{
    LOG(debug) << "transition action switchOn in FilamentCoilControl...";
}

void FilamentCoilControl::handleError(const IFilamentCoilControl::Event&, const IFilamentCoilControl::State&)
{
    LOG(debug) << "transition action handleError in FilamentCoilControl...";
}

void FilamentCoilControl::switchOff(const IFilamentCoilControl::Event&, const IFilamentCoilControl::State&)
{
    LOG(debug) << "transition action switchOff in FilamentCoilControl...";
}

void FilamentCoilControl::startCoil(const IFilamentCoilControl::Event&, const IFilamentCoilControl::State&)
{
    LOG(debug) << "transition action startCoil in FilamentCoilControl...";
}

