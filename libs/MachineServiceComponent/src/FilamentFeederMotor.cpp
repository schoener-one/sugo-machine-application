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

#include "MachineServiceComponent/FilamentFeederMotor.hpp"
#include "HardwareAbstractionLayer/IHardwareAbstractionLayer.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/Protocol.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

FilamentFeederMotor::FilamentFeederMotor(message_broker::IMessageBroker& messageBroker,
                                         common::IProcessContext&        processContext,
                                         const common::ServiceLocator&   serviceLocator)
    : IFilamentFeederMotor(messageBroker, processContext),
      MotorService(hal::id::StepperMotorFeeder, serviceLocator)
{
    m_propertyMotorSpeed.registerValueChangeHandler(
        [this](const IProperty<uint32_t>&) { this->setMotorSpeed(this->m_propertyMotorSpeed); });
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentFeederMotor::switchOn(const IFilamentFeederMotor::Event&,
                                   const IFilamentFeederMotor::State&)
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

void FilamentFeederMotor::startMotor(const IFilamentFeederMotor::Event&,
                                     const IFilamentFeederMotor::State&)
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

void FilamentFeederMotor::handleError(const IFilamentFeederMotor::Event&,
                                      const IFilamentFeederMotor::State&)
{
    stopMotorRotation(true);
    notify(NotificationErrorOccurred);
}

void FilamentFeederMotor::stopMotor(const IFilamentFeederMotor::Event&,
                                    const IFilamentFeederMotor::State&)
{
    stopMotorRotation(false);
    push(Event::StopMotorSucceeded);
    notify(NotificationStopMotorSucceeded);
}

void FilamentFeederMotor::switchOff(const IFilamentFeederMotor::Event&,
                                    const IFilamentFeederMotor::State&)
{
    stopMotorRotation(false);
}
