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
#include "ServiceComponent/IFilamentCoilMotor.hpp"
#include "ServiceComponent/IFilamentFeederMotor.hpp"
#include "ServiceComponent/IFilamentMergerControl.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

MachineControl::MachineControl(message_broker::IMessageBroker& messageBroker,
                               common::IProcessContext&        processContext,
                               const common::ServiceLocator&   serviceLocator)
    : IMachineControl(messageBroker, processContext), m_serviceLocator(serviceLocator)
{
}

///////////////////////////////////////////////////////////////////////////////
// Requests

message_broker::ResponseMessage MachineControl::onCommandRequestIncreaseMotorSpeed(
    const message_broker::Message& request)
{
    m_propertyMotorSpeed.setValue(static_cast<uint32_t>(
        std::clamp(static_cast<int>(m_propertyMotorSpeed.getValue()) +
                       static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                            .getOption(id::ConfigMotorSpeedIncrement)
                                            .get<unsigned>()),
                   0,
                   static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigMotorSpeedMax)
                                        .get<unsigned>()))));
    LOG(debug) << "Increase motor speed to " << m_propertyMotorSpeed.getValue();
    message_broker::ResponseMessage responseMessage{};

    if (!send(IFilamentCoilMotor::PropertyRequestSetMotorSpeed,
              m_propertyMotorSpeed.getValueAsJson(), responseMessage))
    {
        return message_broker::createErrorResponseMessage(request, responseMessage.getResult());
    }

    if (!send(IFilamentFeederMotor::PropertyRequestSetMotorSpeed,
              m_propertyMotorSpeed.getValueAsJson(), responseMessage))
    {
        return message_broker::createErrorResponseMessage(request, responseMessage.getResult());
    }

    return message_broker::createResponseMessage(request);
}

message_broker::ResponseMessage MachineControl::onCommandRequestDecreaseMotorSpeed(
    const message_broker::Message& request)
{
    m_propertyMotorSpeed.setValue(static_cast<uint32_t>(
        std::clamp(static_cast<int>(m_propertyMotorSpeed.getValue()) -
                       static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                            .getOption(id::ConfigMotorSpeedIncrement)
                                            .get<unsigned>()),
                   0,
                   static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigMotorSpeedMax)
                                        .get<unsigned>()))));
    LOG(debug) << "Decrease motor speed to " << m_propertyMotorSpeed.getValue();
    message_broker::ResponseMessage responseMessage{};

    if (!send(IFilamentCoilMotor::PropertyRequestSetMotorSpeed,
              m_propertyMotorSpeed.getValueAsJson(), responseMessage))
    {
        return message_broker::createErrorResponseMessage(request, responseMessage.getResult());
    }

    if (!send(IFilamentFeederMotor::PropertyRequestSetMotorSpeed,
              m_propertyMotorSpeed.getValueAsJson(), responseMessage))
    {
        return message_broker::createErrorResponseMessage(request, responseMessage.getResult());
    }

    return message_broker::createResponseMessage(request);
}

///////////////////////////////////////////////////////////////////////////////
// Notification handler actions

void MachineControl::checkStartingState()
{
    if (m_propertyIsFeedingRunning && m_propertyIsCoilRunning)
    {
        push(Event::StartingSucceeded);
        notify(NotificationRunning);
    }
}

void MachineControl::checkStoppingState()
{
    if (!m_propertyIsFeedingRunning && !m_propertyIsCoilRunning)
    {
        push(Event::StoppingSucceeded);
        notify(NotificationStopped);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions

void MachineControl::switchOn(const IMachineControl::Event&, const IMachineControl::State&)
{
    if (!send(IFilamentMergerControl::CommandRequestSwitchOn))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentCoilControl::CommandRequestSwitchOn))
    {
        push(Event::ErrorOccurred);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void MachineControl::startHeating(const Event&, const State&)
{
    if (!send(IFilamentMergerControl::CommandRequestStartHeating))
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
    m_propertyMotorSpeed.setValue((event == Event::StartHeatless)
                                      ? 0
                                      : m_serviceLocator.get<common::IConfiguration>()
                                            .getOption(id::ConfigMotorSpeedDefault)
                                            .get<unsigned>());

    if (!send(IFilamentFeederMotor::PropertyRequestSetMotorSpeed,
              m_propertyMotorSpeed.getValueAsJson()))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentMergerControl::CommandRequestStartFeeding))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentCoilMotor::PropertyRequestSetMotorSpeed,
              m_propertyMotorSpeed.getValueAsJson()))
    {
        push(Event::ErrorOccurred);
        return;
    }

    const common::Json startCoilParameter(
        {{id::TensionControl, (event == Event::StartHeatless) ? false : true}});

    if (!send(IFilamentCoilControl::CommandRequestStartCoil, startCoilParameter))
    {
        push(Event::ErrorOccurred);
        return;
    }

    notify(NotificationStarting);
}

void MachineControl::handleError(const IMachineControl::Event&, const IMachineControl::State&)
{
    switchOff();
    notify(NotificationErrorOccurred);
}

void MachineControl::stopMachine(const Event&, const State&)
{
    if (!send(IFilamentMergerControl::CommandRequestStopFeeding))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentCoilControl::CommandRequestStopCoil))
    {
        push(Event::ErrorOccurred);
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
