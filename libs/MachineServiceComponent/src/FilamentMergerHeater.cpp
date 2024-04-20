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

#include "MachineServiceComponent/FilamentMergerHeater.hpp"
#include "HardwareAbstractionLayer/Identifier.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/Protocol.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

FilamentMergerHeater::FilamentMergerHeater(message_broker::IMessageBroker& messageBroker,
                                           common::IProcessContext&        processContext,
                                           const common::ServiceLocator&   serviceLocator)
    : IFilamentMergerHeater(messageBroker, processContext),
      HeaterService(hal::id::GpioPinRelaySwitchHeaterMerger, hal::id::TemperatureSensorMerger,
                    id::ConfigMergerHeaterServiceTemperatureMin,
                    id::ConfigMergerHeaterServiceTemperatureMax, serviceLocator)
{
}

///////////////////////////////////////////////////////////////////////////////
// Requests:

message_broker::ResponseMessage FilamentMergerHeater::onPropertyRequestGetTemperature(
    const message_broker::Message& request)
{
    m_propertyTemperature.setValue(getHeaterTemperature());
    return IFilamentMergerHeater::onPropertyRequestGetTemperature(request);
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentMergerHeater::switchOn(const IFilamentMergerHeater::Event&,
                                    const IFilamentMergerHeater::State&)
{
    updateHeaterTemperature();

    if (!m_timerTemperatureObservation.start())
    {
        push(Event::ErrorOccurred);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void FilamentMergerHeater::startHeating(const IFilamentMergerHeater::Event& event,
                                        const IFilamentMergerHeater::State&)
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

void FilamentMergerHeater::stopHeating(const IFilamentMergerHeater::Event& event,
                                       const IFilamentMergerHeater::State&)
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

void FilamentMergerHeater::checkTemperature(const Event&, const State& state)
{
    const auto temperatureState = getHeaterTemperatureState();

    if (TemperatureState::ErrorNoTemperature == temperatureState)
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (State::HeatingOn == state && TemperatureState::AboveMaxTemperature == temperatureState)
    {
        LOG(debug) << "above max temperature: " << getHeaterTemperature() << " °C";
        push(Event::MaxTemperatureReached);
    }
    else if (State::HeatingOff == state &&
             TemperatureState::BelowMinTemperature == temperatureState)
    {
        LOG(debug) << "below min temperature: " << getHeaterTemperature() << " °C";
        push(Event::MinTemperatureReached);
    }
}

void FilamentMergerHeater::switchOff(const IFilamentMergerHeater::Event&,
                                     const IFilamentMergerHeater::State&)
{
    m_timerTemperatureObservation.stop();

    if (!switchHeater(false))
    {
        push(Event::ErrorOccurred);
    }
}

void FilamentMergerHeater::handleError(const IFilamentMergerHeater::Event&,
                                       const IFilamentMergerHeater::State&)
{
    (void)switchHeater(false);
    notify(NotificationErrorOccurred);
}
