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

#include "MachineServiceComponent/FilamentTensionSensor.hpp"
#include "HardwareAbstractionLayer/Identifier.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

FilamentTensionSensor::FilamentTensionSensor(message_broker::IMessageBroker& messageBroker,
                                             common::IProcessContext&        processContext,
                                             const common::ServiceLocator&   serviceLocator)
    : IFilamentTensionSensor(messageBroker, processContext),
      FilamentTensionSensorService(hal::id::GpioPinSignalFilamentTensionLow,
                                   hal::id::GpioPinSignalFilamentTensionHigh,
                                   hal::id::GpioPinSignalFilamentTensionOverload, serviceLocator),
      m_serviceLocator(serviceLocator)
{
}

void FilamentTensionSensor::onFilamentTensionEvent(FilamentTensionEvent event)
{
    if (event == FilamentTensionLow)
    {
        notify(IFilamentTensionSensor::NotificationTensionTooLow);
    }
    else if (event == FilamentTensionHigh)
    {
        notify(IFilamentTensionSensor::NotificationTensionTooHigh);
    }
    else if (event == FilamentTensionOverload)
    {
        notify(IFilamentTensionSensor::NotificationTensionOverloaded);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Requests:

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentTensionSensor::switchOff(const IFilamentTensionSensor::Event&,
                                      const IFilamentTensionSensor::State&)
{
    stopSensorObservation();
}

void FilamentTensionSensor::switchOn(const IFilamentTensionSensor::Event&,
                                     const IFilamentTensionSensor::State&)
{
    if (!startSensorObservation())
    {
        push(Event::SwitchOnFailed);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void FilamentTensionSensor::handleError(const IFilamentTensionSensor::Event&,
                                        const IFilamentTensionSensor::State&)
{
    stopSensorObservation();
    notify(NotificationErrorOccurred);
}
