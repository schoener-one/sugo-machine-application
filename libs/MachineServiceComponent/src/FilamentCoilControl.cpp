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

#include "MachineServiceComponent/FilamentCoilControl.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/Protocol.hpp"
#include "ServiceComponent/IFilamentCoilMotor.hpp"
#include "ServiceComponent/IFilamentTensionSensor.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

FilamentCoilControl::FilamentCoilControl(message_broker::IMessageBroker& messageBroker,
                                         common::IProcessContext&        processContext,
                                         const common::ServiceLocator&   serviceLocator)
    : service_component::IFilamentCoilControl(messageBroker, processContext),
      m_serviceLocator(serviceLocator)
{
}

///////////////////////////////////////////////////////////////////////////////
// Requests:

message_broker::ResponseMessage FilamentCoilControl::onCommandRequestStartCoil(
    const message_broker::Message& request)
{
    const auto  parameters     = common::Json::parse(request.getPayload());
    const auto& tensionControl = parameters.at(id::TensionControl);

    if (tensionControl.empty())
    {
        return message_broker::createErrorResponseMessage(
            request, message_broker::ResponseMessage::Result::InvalidPayload);
    }

    m_controlTension = tensionControl.get<bool>();
    return handleCommandRequestMessage(request, Event::StartMotor);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentCoilControl::switchOn(const IFilamentCoilControl::Event&,
                                   const IFilamentCoilControl::State&)
{
    if (!send(IFilamentCoilMotor::CommandRequestSwitchOn))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (!send(IFilamentTensionSensor::CommandRequestSwitchOn))
    {
        push(Event::ErrorOccurred);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void FilamentCoilControl::switchOff(const IFilamentCoilControl::Event&,
                                    const IFilamentCoilControl::State&)
{
    switchOff();
}

void FilamentCoilControl::switchOff()
{
    switchOffServiceComponent<IFilamentCoilMotor>();
    switchOffServiceComponent<IFilamentTensionSensor>();
}

void FilamentCoilControl::handleError(const IFilamentCoilControl::Event&,
                                      const IFilamentCoilControl::State&)
{
    switchOff();
    notify(NotificationErrorOccurred);
}

void FilamentCoilControl::stopMotor(const IFilamentCoilControl::Event&,
                                    const IFilamentCoilControl::State&)
{
    if (!send(IFilamentCoilMotor::CommandRequestStopMotor))
    {
        push(Event::ErrorOccurred);
    }
}

void FilamentCoilControl::notifyCoilStopped(const IFilamentCoilControl::Event&,
                                            const IFilamentCoilControl::State&)
{
    notify(NotificationCoilStopped);
}

void FilamentCoilControl::startMotor(const IFilamentCoilControl::Event&,
                                     const IFilamentCoilControl::State&)
{
    if (!send(IFilamentCoilMotor::CommandRequestStartMotor))
    {
        push(Event::ErrorOccurred);
    }
}

void FilamentCoilControl::notifyRunning(const IFilamentCoilControl::Event&,
                                        const IFilamentCoilControl::State&)
{
    notify(NotificationCoilRunning);
}

void FilamentCoilControl::controlFilamentTension(const IFilamentCoilControl::Event& event,
                                                 const IFilamentCoilControl::State&)
{
    if (m_controlTension)
    {
        bool success = true;

        if (event == Event::TensionTooLow)
        {
            success = send(IFilamentCoilMotor::CommandRequestIncreaseMotorOffsetSpeed);
        }
        else if (event == Event::TensionTooHigh)
        {
            success = send(IFilamentCoilMotor::CommandRequestDecreaseMotorOffsetSpeed);
        }

        if (!success)
        {
            push(Event::ErrorOccurred);
        }
    }
    else
    {
        LOG(debug) << "Ignoring tension event " << event
                   << " because tension control is turned off";
    }
}