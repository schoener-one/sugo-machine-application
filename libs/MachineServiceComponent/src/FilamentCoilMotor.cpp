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

#include "MachineServiceComponent/FilamentCoilMotor.hpp"
#include "HardwareAbstractionLayer/IHardwareAbstractionLayer.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/Protocol.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

FilamentCoilMotor::FilamentCoilMotor(message_broker::IMessageBroker& messageBroker,
                                     common::IProcessContext&        processContext,
                                     const common::ServiceLocator&   serviceLocator)
    : IFilamentCoilMotor(messageBroker, processContext),
      MotorService(hal::id::StepperMotorCoiler, serviceLocator),
      m_serviceLocator(serviceLocator)
{
}

///////////////////////////////////////////////////////////////////////////////
// Requests:

message_broker::ResponseMessage FilamentCoilMotor::onRequestSetMotorSpeed(
    const message_broker::Message& request)
{
    const auto  parameters = common::Json::parse(request.getPayload());
    const auto& motorSpeed = parameters.at(id::Speed);

    if (motorSpeed.empty())
    {
        return message_broker::createErrorResponseMessage(
            request, message_broker::ResponseMessage::Result::InvalidPayload);
    }

    (void)setMotorSpeed(motorSpeed.get<unsigned>());
    return message_broker::createResponseMessage(request);
}

message_broker::ResponseMessage FilamentCoilMotor::onRequestIncreaseMotorOffsetSpeed(
    const message_broker::Message& request)
{
    const int motorSpeedIncrement = static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                                         .getOption(id::ConfigMotorSpeedIncrement)
                                                         .get<unsigned>());
    (void)addMotorOffsetSpeed(motorSpeedIncrement);
    return message_broker::createResponseMessage(request);
}

message_broker::ResponseMessage FilamentCoilMotor::onRequestDecreaseMotorOffsetSpeed(
    const message_broker::Message& request)
{
    static constexpr int negativeMultiplier = -1;
    const int motorSpeedIncrement = static_cast<int>(m_serviceLocator.get<common::IConfiguration>()
                                                         .getOption(id::ConfigMotorSpeedIncrement)
                                                         .get<unsigned>());
    (void)addMotorOffsetSpeed(negativeMultiplier * motorSpeedIncrement);
    return message_broker::createResponseMessage(request);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentCoilMotor::switchOn(const IFilamentCoilMotor::Event&, const IFilamentCoilMotor::State&)
{
    if (resetMotor())
    {
        push(Event::SwitchOnSucceeded);
    }
    else
    {
        push(Event::ErrorOccurred);
    }
}

void FilamentCoilMotor::startMotor(const IFilamentCoilMotor::Event&,
                                   const IFilamentCoilMotor::State&)
{
    if (startMotorRotation())
    {
        push(Event::StartMotorSucceeded);
        notify(NotificationStartMotorSucceeded);
    }
    else
    {
        push(Event::ErrorOccurred);
        notify(NotificationErrorOccurred);
    }
}

void FilamentCoilMotor::handleError(const IFilamentCoilMotor::Event&,
                                    const IFilamentCoilMotor::State&)
{
    stopMotorRotation(true);
    notify(NotificationErrorOccurred);
}

void FilamentCoilMotor::stopMotor(const IFilamentCoilMotor::Event&,
                                  const IFilamentCoilMotor::State&)
{
    stopMotorRotation(false);
    push(Event::StopMotorSucceeded);
    notify(NotificationStopMotorSucceeded);
}

void FilamentCoilMotor::switchOff(const IFilamentCoilMotor::Event&,
                                  const IFilamentCoilMotor::State&)
{
    stopMotorRotation(false);
}
