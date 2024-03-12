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

#include "MachineServiceComponent/FilamentMergerControl.hpp"
#include "ServiceComponent/IFilamentFeederMotor.hpp"
#include "ServiceComponent/IFilamentMergerHeater.hpp"
#include "ServiceComponent/IFilamentPreHeater.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

FilamentMergerControl::FilamentMergerControl(message_broker::IMessageBroker& messageBroker,
                                             common::IProcessContext&        processContext,
                                             const common::ServiceLocator&   serviceLocator)
    : IFilamentMergerControl(messageBroker, processContext), m_serviceLocator(serviceLocator)
{
}

///////////////////////////////////////////////////////////////////////////////
// Requests:

void FilamentMergerControl::onNotificationFilamentPreHeaterTargetTemperatureRangeReached(
    const message_broker::Message&)
{
    const bool stateChange          = !m_isPreHeaterTemperatureReached;
    m_isPreHeaterTemperatureReached = true;

    // TODO add support for state change conditions!
    if (stateChange && m_isMergerHeaterTemperatureReached && m_isPreHeaterTemperatureReached)
    {
        handleNotificationMessage(Event::HeatingUpSucceeded);
    }
}

void FilamentMergerControl::onNotificationFilamentMergerHeaterTargetTemperatureRangeReached(
    const message_broker::Message&)
{
    const bool stateChange             = !m_isMergerHeaterTemperatureReached;
    m_isMergerHeaterTemperatureReached = true;

    // TODO add support for state change conditions!
    if (stateChange && m_isMergerHeaterTemperatureReached && m_isPreHeaterTemperatureReached)
    {
        handleNotificationMessage(Event::HeatingUpSucceeded);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentMergerControl::switchOn(const IFilamentMergerControl::Event&,
                                     const IFilamentMergerControl::State&)
{
    m_isMergerHeaterTemperatureReached = false;
    m_isPreHeaterTemperatureReached    = false;

    if (!send(IFilamentFeederMotor::CommandRequestSwitchOn))
    {
        push(Event::ErrorOccurred);
    }
    else
    {
        push(Event::SwitchOnSucceeded);
    }
}

void FilamentMergerControl::switchOff(const IFilamentMergerControl::Event&,
                                      const IFilamentMergerControl::State&)
{
    switchOff();
}

void FilamentMergerControl::switchOff()
{
    switchOffServiceComponent<IFilamentFeederMotor>();
    switchOffServiceComponent<IFilamentMergerHeater>();
    switchOffServiceComponent<IFilamentPreHeater>();
}

void FilamentMergerControl::startMotor(const IFilamentMergerControl::Event&,
                                       const IFilamentMergerControl::State&)
{
    if (!send(IFilamentFeederMotor::CommandRequestStartMotor))
    {
        push(Event::ErrorOccurred);
    }
}

void FilamentMergerControl::stopMerger(const IFilamentMergerControl::Event&,
                                       const IFilamentMergerControl::State&)
{
    switchOffServiceComponent<IFilamentMergerHeater>();
    switchOffServiceComponent<IFilamentPreHeater>();

    if (!send(IFilamentFeederMotor::CommandRequestStopMotor))
    {
        push(Event::ErrorOccurred);
    }
    notify(NotificationFeedingStopped);
}

void FilamentMergerControl::handleError(const IFilamentMergerControl::Event&,
                                        const IFilamentMergerControl::State&)
{
    switchOff();
    notify(NotificationErrorOccurred);
}

void FilamentMergerControl::notifyHeatedUp(const Event&, const State&)
{
    notify(NotificationHeatedUp);
}

void FilamentMergerControl::heatingUp(const IFilamentMergerControl::Event&,
                                      const IFilamentMergerControl::State&)
{
    m_isMergerHeaterTemperatureReached = false;
    m_isPreHeaterTemperatureReached    = false;

    if (!send(IFilamentPreHeater::CommandRequestSwitchOn))
    {
        push(Event::ErrorOccurred);
    }

    if (!send(IFilamentMergerHeater::CommandRequestSwitchOn))
    {
        push(Event::ErrorOccurred);
    }
}

void FilamentMergerControl::notifyRunning(const IFilamentMergerControl::Event&,
                                          const IFilamentMergerControl::State&)
{
    notify(NotificationFeedingRunning);
}