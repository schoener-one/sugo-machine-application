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
#include "MachineConfig.hpp"
#include "MachineProtocol.hpp"

#include <algorithm>

using namespace std;
using namespace sugo;

MachineControl::MachineControl(message::ICommandMessageBroker& messageBroker,
                               const ServiceLocator&           serviceLocator)
    : IMachineControl(messageBroker),
      m_serviceLocator(serviceLocator),
      m_motorSpeed(
          m_serviceLocator.get<IConfiguration>().getOption(id::MotorSpeedDefault).get<unsigned>())
{
}

///////////////////////////////////////////////////////////////////////////////
// Commands
message::CommandResponse MachineControl::onCommandSwitchOn(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOn);
}

message::CommandResponse MachineControl::onCommandSwitchOff(const message::Command& command)
{
    return handleEventMessage(command, Event::SwitchOff);
}

message::CommandResponse MachineControl::onCommandStart(const message::Command& command)
{
    return handleEventMessage(command, Event::Start);
}

message::CommandResponse MachineControl::onCommandStartHeatless(const message::Command& command)
{
    return handleEventMessage(command, Event::StartHeatless);
}

message::CommandResponse MachineControl::onCommandIncreaseMotorSpeed(const message::Command&)
{
    m_motorSpeed = static_cast<unsigned>(std::clamp(
        static_cast<int>(m_motorSpeed) + static_cast<int>(m_serviceLocator.get<IConfiguration>()
                                                              .getOption(id::MotorSpeedIncrement)
                                                              .get<unsigned>()),
        0,
        static_cast<int>(
            m_serviceLocator.get<IConfiguration>().getOption(id::MotorSpeedMax).get<unsigned>())));
    LOG(debug) << "Increase motor speed to " << m_motorSpeed;
    const Json parameters({{protocol::IdSpeed, m_motorSpeed}});
    auto       responseCoilControl = send(IFilamentCoilControl::CommandSetMotorSpeed, parameters);
    if (responseCoilControl.result() != message::CommandResponse_Result_SUCCESS)
    {
        return responseCoilControl;
    }
    return send(IFilamentMergerControl::CommandSetMotorSpeed, parameters);
}

message::CommandResponse MachineControl::onCommandDecreaseMotorSpeed(const message::Command&)
{
    m_motorSpeed = static_cast<unsigned>(std::clamp(
        static_cast<int>(m_motorSpeed) - static_cast<int>(m_serviceLocator.get<IConfiguration>()
                                                              .getOption(id::MotorSpeedIncrement)
                                                              .get<unsigned>()),
        0,
        static_cast<int>(
            m_serviceLocator.get<IConfiguration>().getOption(id::MotorSpeedMax).get<unsigned>())));
    LOG(debug) << "Decrease motor speed to " << m_motorSpeed;
    const Json parameters({{protocol::IdSpeed, m_motorSpeed}});
    auto       responseCoilControl = send(IFilamentCoilControl::CommandSetMotorSpeed, parameters);
    if (responseCoilControl.result() != message::CommandResponse_Result_SUCCESS)
    {
        return responseCoilControl;
    }
    return send(IFilamentMergerControl::CommandSetMotorSpeed, parameters);
}

message::CommandResponse MachineControl::onCommandStop(const message::Command& command)
{
    return handleEventMessage(command, Event::Stop);
}

message::CommandResponse MachineControl::onCommandGetMotorSpeed(const message::Command& command)
{
    return createCommandResponse(command, Json({{protocol::IdSpeed, m_motorSpeed}}));
}

message::CommandResponse MachineControl::onNotificationFilamentMergerControlFeedingRunning(
    const message::Command& command)
{
    m_isFilamentMergerControlRunning = true;
    return handleEventMessage(command, Event::CheckStartingState);
}

message::CommandResponse MachineControl::onNotificationFilamentMergerControlFeedingStopped(
    const message::Command& command)
{
    m_isFilamentMergerControlRunning = false;
    return handleEventMessage(command, Event::CheckStoppingState);
}

message::CommandResponse MachineControl::onNotificationFilamentMergerControlHeatedUp(
    const message::Command& command)
{
    return handleEventMessage(command, Event::MergerHeatedUp);
}

message::CommandResponse MachineControl::onNotificationFilamentMergerControlErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event::ErrorOccurred);
}

message::CommandResponse MachineControl::onNotificationFilamentCoilControlCoilRunning(
    const message::Command& command)
{
    m_isFilamentCoilControlRunning = true;
    return handleEventMessage(command, Event::CheckStartingState);
}

message::CommandResponse MachineControl::onNotificationFilamentCoilControlCoilStopped(
    const message::Command& command)
{
    m_isFilamentCoilControlRunning = false;
    return handleEventMessage(command, Event::CheckStoppingState);
}

message::CommandResponse MachineControl::onNotificationFilamentCoilControlErrorOccurred(
    const message::Command& command)
{
    return handleEventMessage(command, Event::ErrorOccurred);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions
void MachineControl::switchOn(const IMachineControl::Event&, const IMachineControl::State&)
{
    const auto responseFilamentMerger = send(IFilamentMergerControl::CommandSwitchOn);
    if (responseFilamentMerger.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::SwitchOnFailed);
        return;
    }
    const auto responseFilamentCoilControl = send(IFilamentCoilControl::CommandSwitchOn);
    if (responseFilamentCoilControl.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::SwitchOnFailed);
        return;
    }
    push(Event::SwitchOnSucceeded);
}

void MachineControl::startHeating(const Event&, const State&)
{
    const auto response = send(IFilamentMergerControl::CommandStartHeating);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        push(Event::ErrorOccurred);
    }

    notify(NotificationHeatingUp);
}

void MachineControl::startMachine(const IMachineControl::Event& event,
                                  const IMachineControl::State&)
{
    m_isFilamentMergerControlRunning = false;
    m_isFilamentCoilControlRunning   = false;
    m_motorSpeed                     = (event == Event::StartHeatless) ? 0
                                                   : m_serviceLocator.get<IConfiguration>()
                                                         .getOption(id::MotorSpeedDefault)
                                                         .get<unsigned>();

    const auto responseSetMotorSpeedFeeding = send(IFilamentMergerControl::CommandSetMotorSpeed,
                                                   Json({{protocol::IdSpeed, m_motorSpeed}}));
    const auto responseStartFeeding         = send(IFilamentMergerControl::CommandStartFeeding);

    if ((responseSetMotorSpeedFeeding.result() != message::CommandResponse_Result_SUCCESS) ||
        (responseStartFeeding.result() != message::CommandResponse_Result_SUCCESS))
    {
        push(Event::ErrorOccurred);
        return;
    }

    const auto responseSetMotorSpeedCoil =
        send(IFilamentCoilControl::CommandSetMotorSpeed, Json({{protocol::IdSpeed, m_motorSpeed}}));
    const auto responseStartCoil =
        send(IFilamentCoilControl::CommandStartCoil,
             Json({{protocol::IdTensionControl, (event == Event::StartHeatless) ? false : true}}));

    if ((responseSetMotorSpeedCoil.result() != message::CommandResponse_Result_SUCCESS) ||
        (responseStartCoil.result() != message::CommandResponse_Result_SUCCESS))
    {
        send(IFilamentMergerControl::CommandStopFeeding);
        push(Event::ErrorOccurred);
        return;
    }

    notify(NotificationStarting);
}

void MachineControl::checkStartingState(const Event&, const State&)
{
    if (m_isFilamentMergerControlRunning && m_isFilamentCoilControlRunning)
    {
        push(Event::StartingSucceeded);
        notify(NotificationRunning);
    }
}

void MachineControl::handleError(const IMachineControl::Event&, const IMachineControl::State&)
{
    switchOff();
    notify(NotificationErrorOccurred);
}

void MachineControl::stopMachine(const Event&, const State&)
{
    const auto responseStopMerger = send(IFilamentMergerControl::CommandStopFeeding);
    const auto responseStopCoil   = send(IFilamentCoilControl::CommandStopCoil);

    if ((responseStopMerger.result() != message::CommandResponse_Result_SUCCESS) ||
        (responseStopCoil.result() != message::CommandResponse_Result_SUCCESS))
    {
        push(Event::ErrorOccurred);
    }
}

void MachineControl::checkStoppingState(const Event&, const State&)
{
    if (!m_isFilamentMergerControlRunning && !m_isFilamentCoilControlRunning)
    {
        push(Event::StoppingSucceeded);
        notify(NotificationStopped);
    }
}

void MachineControl::notifyStopped(const Event&, const State&)
{
    notify(NotificationStopped);
}

void MachineControl::switchOff(const IMachineControl::Event&, const IMachineControl::State&)
{
    switchOff();
    notify(NotificationSwitchedOff);
}

void MachineControl::switchOff()
{
    switchOffServiceComponent<IFilamentMergerControl>();
    switchOffServiceComponent<IFilamentCoilControl>();
}
