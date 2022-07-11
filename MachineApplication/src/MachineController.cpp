///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.09.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include "MachineController.hpp"
#include "Globals.hpp"
#include "IFilamentCoilControl.hpp"
#include "IFilamentMerger.hpp"

using namespace std;
using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands
message::CommandResponse MachineController::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse MachineController::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse MachineController::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions
void MachineController::switchOn(const IMachineController::Event&, const IMachineController::State&)
{
    const auto responseFilamentMerger = send(IFilamentMerger::CommandSwitchOn);
    if (responseFilamentMerger.result() == message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::SwitchOnSucceeded));
    }
    else
    {
        push(Event(EventId::SwitchOnFailed));
    }
    const auto responseFilamentCoilControl = send(IFilamentCoilControl::CommandSwitchOn);
    if (responseFilamentCoilControl.result() == message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::SwitchOnSucceeded));
    }
    else
    {
        push(Event(EventId::SwitchOnFailed));
    }
}
void MachineController::startMachine(const IMachineController::Event&,
                                     const IMachineController::State&)
{
    m_isFilamentMergerStarted = false;
    m_isFilamentCoilControlStarted = false;
    push(Event(EventId::WaitForStarted));
    sendAsync(IFilamentMerger::CommandStartMotor);
    sendAsync(IFilamentCoilControl::CommandStartMotor);
}

void MachineController::waitForStarted(const IMachineController::Event& event, const IMachineController::State& state)
{
    if (m_isFilamentMergerStarted && m_isFilamentCoilControlStarted)
    {
        push(Event(EventId::StartingSucceeded));
    }
}

void MachineController::handleError(const IMachineController::Event& event,
                                    const IMachineController::State& state)
{
    LOG(error) << "Error occurred...";
    switchOff(event, state);
}

void MachineController::switchOff(const IMachineController::Event&,
                                  const IMachineController::State&)
{
    (void)send(IFilamentMerger::CommandSwitchOff);
    (void)send(IFilamentCoilControl::CommandSwitchOff);
}

void MachineController::runSelfTest(const IMachineController::Event&,
                                    const IMachineController::State&)
{
    // not implemented yet!
    push(Event(EventId::SelfTestSucceeded));
}
