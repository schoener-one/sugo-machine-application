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

#include "MachineServiceComponent/FilamentPreHeater.hpp"
#include "MachineServiceComponent/Protocol.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

FilamentPreHeater::FilamentPreHeater(message_broker::IMessageBroker& messageBroker,
                                     common::IProcessContext&        processContext,
                                     const common::ServiceLocator&   serviceLocator)
    : IFilamentPreHeater(messageBroker, processContext),
      HeaterService(hal::id::GpioPinRelaySwitchHeaterFeeder, hal::id::TemperatureSensorFeeder,
                    serviceLocator)
{
}

void FilamentPreHeater::onTemperatureLimitEvent(TemperatureLimitEvent event)
{
    switch (event)
    {
        case MinTemperatureReached:
            push(Event::MinTemperatureReached);
            break;
        case MaxTemperatureReached:
            push(Event::MaxTemperatureReached);
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Requests:

message_broker::ResponseMessage FilamentPreHeater::onRequestGetTemperature(
    const message_broker::Message& request)
{
    return message_broker::createResponseMessage(
        request, common::Json({{id::Temperature, getTemperature()}}));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentPreHeater::switchOn(const IFilamentPreHeater::Event&, const IFilamentPreHeater::State&)
{
    updateHeaterTemperature();

    if (!startTemperatureObservation())
    {
        push(Event::ErrorOccurred);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void FilamentPreHeater::startHeating(const IFilamentPreHeater::Event& event,
                                     const IFilamentPreHeater::State&)
{
    if (!switchHeater(true))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (event == Event::MinTemperatureReached)
    {
        notify(NotificationTargetTemperatureRangeLeft);
    }
}

void FilamentPreHeater::stopHeating(const IFilamentPreHeater::Event& event,
                                    const IFilamentPreHeater::State&)
{
    if (!switchHeater(false))
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (event == Event::MaxTemperatureReached)
    {
        notify(NotificationTargetTemperatureRangeReached);
    }
}

void FilamentPreHeater::switchOff(const IFilamentPreHeater::Event&,
                                  const IFilamentPreHeater::State&)
{
    stopTemperatureObservation();

    if (!switchHeater(false))
    {
        push(Event::ErrorOccurred);
    }
}

void FilamentPreHeater::handleError(const IFilamentPreHeater::Event&,
                                    const IFilamentPreHeater::State&)
{
    (void)switchHeater(false);
    notify(NotificationErrorOccurred);
}
