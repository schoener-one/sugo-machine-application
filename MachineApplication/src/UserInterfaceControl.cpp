
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceControl.hpp"

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse UserInterfaceControl::onCommandMachineControlStarting(
    const message::Command&)
{
    LOG(debug) << "command MachineControl.Starting received in UserInterfaceControl...";
    return message::CommandResponse();
}

message::CommandResponse UserInterfaceControl::onCommandMachineControlRunning(
    const message::Command&)
{
    LOG(debug) << "command MachineControl.Running received in UserInterfaceControl...";
    return message::CommandResponse();
}

message::CommandResponse UserInterfaceControl::onCommandMachineControlSwitchedOff(
    const message::Command&)
{
    LOG(debug) << "command MachineControl.SwitchedOff received in UserInterfaceControl...";
    return message::CommandResponse();
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void UserInterfaceControl::switchOff(const IUserInterfaceControl::Event&,
                                     const IUserInterfaceControl::State&)
{
    LOG(debug) << "transition action switchOff in UserInterfaceControl...";
}

void UserInterfaceControl::switchOn(const IUserInterfaceControl::Event&,
                                    const IUserInterfaceControl::State&)
{
    LOG(debug) << "transition action switchOn in UserInterfaceControl...";
}

void UserInterfaceControl::handleError(const IUserInterfaceControl::Event&,
                                       const IUserInterfaceControl::State&)
{
    LOG(debug) << "transition action handleError in UserInterfaceControl...";
}
