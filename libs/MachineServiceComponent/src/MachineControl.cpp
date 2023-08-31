///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>

#include "Common/Types.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/MachineControl.hpp"
#include "MachineServiceComponent/Protocol.hpp"
#include "ServiceComponent/IFilamentCoilControl.hpp"
#include "ServiceComponent/IFilamentMergerControl.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

MachineControl::MachineControl(message_broker::IMessageBroker& messageBroker,
                               common::IProcessContext&        processContext,
                               const common::ServiceLocator&   serviceLocator)
    : IMachineControl(messageBroker, processContext),
      m_serviceLocator(serviceLocator),
      m_motorSpeed(m_serviceLocator.get<common::IConfiguration>()
                       .getOption(id::ConfigMotorSpeedDefault)
                       .get<unsigned>())
{
}

///////////////////////////////////////////////////////////////////////////////
// Requests

message_broker::ResponseMessage MachineControl::onRequestIncreaseMotorSpeed(
    const message_broker::Message& request)
{
    m_motorSpeed = static_cast<unsigned>(
        std::clamp(static_cast<int>(m_motorSpeed) +
                       static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                            .getOption(id::ConfigMotorSpeedIncrement)
                                            .get<unsigned>()),
                   0,
                   static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigMotorSpeedMax)
                                        .get<unsigned>())));
    LOG(debug) << "Increase motor speed to " << m_motorSpeed;
    const common::Json              parameters({{id::Speed, m_motorSpeed}});
    message_broker::ResponseMessage responseMessage{};

    if (!send(IFilamentCoilControl::RequestSetMotorSpeed, parameters, responseMessage))
    {
        return message_broker::createErrorResponseMessage(request, responseMessage.getResult());
    }

    if (!send(IFilamentMergerControl::RequestSetMotorSpeed, parameters, responseMessage))
    {
        return message_broker::createErrorResponseMessage(request, responseMessage.getResult());
    }

    return message_broker::createResponseMessage(request);
}

message_broker::ResponseMessage MachineControl::onRequestDecreaseMotorSpeed(
    const message_broker::Message& request)
{
    m_motorSpeed = static_cast<unsigned>(
        std::clamp(static_cast<int>(m_motorSpeed) -
                       static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                            .getOption(id::ConfigMotorSpeedIncrement)
                                            .get<unsigned>()),
                   0,
                   static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigMotorSpeedMax)
                                        .get<unsigned>())));
    LOG(debug) << "Decrease motor speed to " << m_motorSpeed;
    const common::Json              parameters({{id::Speed, m_motorSpeed}});
    message_broker::ResponseMessage responseMessage{};

    if (!send(IFilamentCoilControl::RequestSetMotorSpeed, parameters, responseMessage))
    {
        return message_broker::createErrorResponseMessage(request, responseMessage.getResult());
    }

    if (!send(IFilamentMergerControl::RequestSetMotorSpeed, parameters, responseMessage))
    {
        return message_broker::createErrorResponseMessage(request, responseMessage.getResult());
    }

    return message_broker::createResponseMessage(request);
}

message_broker::ResponseMessage MachineControl::onRequestGetMotorSpeed(
    const message_broker::Message& request)
{
    return message_broker::createResponseMessage(request,
                                                 common::Json({{id::Speed, m_motorSpeed}}));
}

void MachineControl::onNotificationFilamentMergerControlFeedingRunning(
    const message_broker::Message& request)
{
    m_isFilamentMergerControlRunning = true;
    handleEventMessage(request, Event::CheckStartingState);
}

void MachineControl::onNotificationFilamentMergerControlFeedingStopped(
    const message_broker::Message& request)
{
    m_isFilamentMergerControlRunning = false;
    handleEventMessage(request, Event::CheckStoppingState);
}

void MachineControl::onNotificationFilamentCoilControlCoilRunning(
    const message_broker::Message& request)
{
    m_isFilamentCoilControlRunning = true;
    handleEventMessage(request, Event::CheckStartingState);
}

void MachineControl::onNotificationFilamentCoilControlCoilStopped(
    const message_broker::Message& request)
{
    m_isFilamentCoilControlRunning = false;
    handleEventMessage(request, Event::CheckStoppingState);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions

void MachineControl::switchOn(const IMachineControl::Event&, const IMachineControl::State&)
{
    if (!send(IFilamentMergerControl::RequestSwitchOn))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentCoilControl::RequestSwitchOn))
    {
        push(Event::ErrorOccurred);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void MachineControl::startHeating(const Event&, const State&)
{
    if (!send(IFilamentMergerControl::RequestStartHeating))
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
                                                   : m_serviceLocator.get<common::IConfiguration>()
                                                         .getOption(id::ConfigMotorSpeedDefault)
                                                         .get<unsigned>();
    const common::Json setMotorSpeedParameter({{id::Speed, m_motorSpeed}});

    if (!send(IFilamentMergerControl::RequestSetMotorSpeed, setMotorSpeedParameter))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentMergerControl::RequestStartFeeding))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentCoilControl::RequestSetMotorSpeed, setMotorSpeedParameter))
    {
        push(Event::ErrorOccurred);
        return;
    }

    const common::Json startCoilParameter(
        {{id::TensionControl, (event == Event::StartHeatless) ? false : true}});

    if (!send(IFilamentCoilControl::RequestStartCoil, startCoilParameter))
    {
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
    if (!send(IFilamentMergerControl::RequestStopFeeding))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentCoilControl::RequestStopCoil))
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
