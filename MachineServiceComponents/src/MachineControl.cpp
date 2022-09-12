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
#include "MachineConfig.hpp"
#include "MachineProtocol.hpp"

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

message::CommandResponse MachineControl::onCommandIncreaseMotorSpeed(const message::Command&)
{
    m_motorSpeed = ((m_motorSpeed + config::MotorSpeedInc) % config::MaxMotorSpeed);
    LOG(debug) << "Increase motor speed to " << m_motorSpeed;
    const Json parameters({protocol::IdSpeed, m_motorSpeed});
    const auto responseCoilControl = send(IFilamentCoilControl::CommandSetMotorSpeed, parameters);
    if (responseCoilControl.result() != message::CommandResponse_Result_SUCCESS)
    {
        return responseCoilControl;
    }
    return send(IFilamentMergerControl::CommandSetMotorSpeed, parameters);
}

message::CommandResponse MachineControl::onCommandDecreaseMotorSpeed(const message::Command&)
{
    m_motorSpeed = ((m_motorSpeed - config::MotorSpeedInc) % config::MaxMotorSpeed);
    LOG(debug) << "Decrease motor speed to " << m_motorSpeed;
    const Json parameters({protocol::IdSpeed, m_motorSpeed});
    const auto responseCoilControl = send(IFilamentCoilControl::CommandSetMotorSpeed, parameters);
    if (responseCoilControl.result() != message::CommandResponse_Result_SUCCESS)
    {
        return responseCoilControl;
    }
    return send(IFilamentMergerControl::CommandSetMotorSpeed, parameters);
}

message::CommandResponse MachineControl::onCommandGetMotorSpeed(const message::Command& command)
{
    return createResponse(command, Json{{protocol::IdSpeed, m_motorSpeed}});
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
    return handleStateChangeCommand(command, Event(EventId::RunningStopped));
}

message::CommandResponse MachineControl::onCommandFilamentMergerControlHeatedUp(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::MergerHeatedUp));
}

message::CommandResponse MachineControl::onCommandFilamentMergerControlErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::ErrorOccurred));
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
    return handleStateChangeCommand(command, Event(EventId::RunningStopped));
}

message::CommandResponse MachineControl::onCommandFilamentCoilControlErrorOccurred(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::ErrorOccurred));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions
void MachineControl::switchOn(const IMachineControl::Event&, const IMachineControl::State&)
{
    m_motorSpeed                      = config::DefaultMotorSpeed;
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
    notify(NotificationStarting);
}

void MachineControl::startMachine(const IMachineControl::Event&, const IMachineControl::State&)
{
    m_isFilamentMergerControlRunning = false;
    m_isFilamentCoilControlRunning   = false;
    const auto responseStartFeeding  = send(IFilamentMergerControl::CommandStartFeeding,
                                           Json({{protocol::IdSpeed, m_motorSpeed}}));
    if (responseStartFeeding.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event(EventId::StartingFailed));
        return;
    }
    const auto responseStartCoil = send(IFilamentCoilControl::CommandStartCoil);
    if (responseStartCoil.result() != message::CommandResponse_Result_SUCCESS)
    {
        (void)send(IFilamentMergerControl::CommandStopFeeding);
        push(Event(EventId::StartingFailed));
        return;
    }
    push(Event(EventId::WaitForStarted));
}

void MachineControl::waitForStarted(const IMachineControl::Event&, const IMachineControl::State&)
{
    if (m_isFilamentMergerControlRunning && m_isFilamentCoilControlRunning)
    {
        push(Event(EventId::StartingSucceeded));
        notify(NotificationRunning);
    }
}

void MachineControl::handleError(const IMachineControl::Event& event,
                                 const IMachineControl::State& state)
{
    LOG(error) << "Error occurred...";
    switchOff(event, state);
    notify(NotificationError);
}

void MachineControl::switchOff(const IMachineControl::Event&, const IMachineControl::State&)
{
    (void)send(IFilamentMergerControl::CommandSwitchOff);
    (void)send(IFilamentCoilControl::CommandSwitchOff);
    notify(NotificationSwitchedOff);
}

void MachineControl::runSelfTest(const IMachineControl::Event&, const IMachineControl::State&)
{
    // TODO not implemented yet!
    push(Event(EventId::SelfTestSucceeded));
}
