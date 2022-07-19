///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.09.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include "MachineControl.hpp"
#include "Globals.hpp"
#include "IFilamentCoilControl.hpp"
#include "IFilamentMergerControl.hpp"
#include "IUserInterfaceControl.hpp"

using namespace std;
using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands
message::CommandResponse MachineControl::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse MachineControl::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse MachineControl::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command);
}

message::CommandResponse MachineControl::onCommandIncreaseMotorSpeed(
    const message::Command& command)
{
    // forwarding
    const auto responseCoilControl = send(IFilamentCoilControl::CommandIncreaseMotorSpeed, command.parameters());
    if (responseCoilControl.result() != message::CommandResponse_Result_SUCCESS)
    {
        return responseCoilControl;
    }
    return send(IFilamentMergerControl::CommandIncreaseMotorSpeed, command.parameters());
}

message::CommandResponse MachineControl::onCommandDecreaseMotorSpeed(
    const message::Command& command)
{
    // forwarding
    const auto responseCoilControl = send(IFilamentCoilControl::CommandDecreaseMotorSpeed, command.parameters());
    if (responseCoilControl.result() != message::CommandResponse_Result_SUCCESS)
    {
        return responseCoilControl;
    }
    return send(IFilamentMergerControl::CommandDecreaseMotorSpeed, command.parameters());
}

message::CommandResponse MachineControl::onCommandFilamentMergerControlFeedingRunning(
    const message::Command& command)
{
    m_isFilamentMergerControlRunning = true;
    return handleStateChangeCommand(command, Event(EventId::WaitForStarted));
}

message::CommandResponse MachineControl::onCommandFilamentMergerControlFeedingStopped(
    const message::Command& command)
{
    m_isFilamentMergerControlRunning = false;
    return handleStateChangeCommand(command, Event(EventId::RunningFailed));
}

message::CommandResponse MachineControl::onCommandFilamentMergerControlErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::RunningFailed));
}

message::CommandResponse MachineControl::onCommandFilamentCoilControlCoilRunning(
    const message::Command& command)
{
    m_isFilamentCoilControlRunning = true;
    return handleStateChangeCommand(command, Event(EventId::WaitForStarted));
}

message::CommandResponse MachineControl::onCommandFilamentCoilControlCoilStopped(
    const message::Command& command)
{
    m_isFilamentCoilControlRunning = false;
    return handleStateChangeCommand(command, Event(EventId::RunningFailed));
}

message::CommandResponse MachineControl::onCommandFilamentCoilControlErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::RunningFailed));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions
void MachineControl::switchOn(const IMachineControl::Event&, const IMachineControl::State&)
{
    const auto responseFilamentMerger = send(IFilamentMergerControl::CommandSwitchOn);
    if (responseFilamentMerger.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::SwitchOnFailed));
        return;
    }
    const auto responseFilamentCoilControl = send(IFilamentCoilControl::CommandSwitchOn);
    if (responseFilamentCoilControl.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::SwitchOnFailed));
        return;
    }

    push(Event(EventId::SwitchOnSucceeded));
}

void MachineControl::startMachine(const IMachineControl::Event&, const IMachineControl::State&)
{
    m_isFilamentMergerControlRunning = false;
    m_isFilamentCoilControlRunning   = false;
    push(Event(EventId::WaitForStarted));
    send(IFilamentMergerControl::CommandStartFeeding);
    send(IFilamentCoilControl::CommandStartCoil);
}

void MachineControl::waitForStarted(const IMachineControl::Event&, const IMachineControl::State&)
{
    if (m_isFilamentMergerControlRunning && m_isFilamentCoilControlRunning)
    {
        push(Event(EventId::StartingSucceeded));
        notify(IMachineControl::NotificationRunning);
    }
}

void MachineControl::handleError(const IMachineControl::Event& event,
                                 const IMachineControl::State& state)
{
    LOG(error) << "Error occurred...";
    switchOff(event, state);
}

void MachineControl::switchOff(const IMachineControl::Event&, const IMachineControl::State&)
{
    (void)send(IFilamentMergerControl::CommandSwitchOff);
    (void)send(IFilamentCoilControl::CommandSwitchOff);
}

void MachineControl::runSelfTest(const IMachineControl::Event&, const IMachineControl::State&)
{
    // not implemented yet!
    push(Event(EventId::SelfTestSucceeded));
}
