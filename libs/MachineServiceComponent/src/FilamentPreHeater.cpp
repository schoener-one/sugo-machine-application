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
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/Protocol.hpp"

using namespace sugo;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

FilamentPreHeater::FilamentPreHeater(message_broker::IMessageBroker& messageBroker,
                                     common::IProcessContext&        processContext,
                                     const common::ServiceLocator&   serviceLocator)
    : IFilamentPreHeater(messageBroker, processContext),
      HeaterService(hal::id::GpioPinRelaySwitchHeaterFeeder, hal::id::TemperatureSensorFeeder,
                    id::ConfigPreHeaterServiceTemperatureMin,
                    id::ConfigPreHeaterServiceTemperatureMax, serviceLocator)
{
    m_propertyTemperature.registerValueChangeHandler([this](const IProperty<int32_t>& temperature) {
        // TODO automate creation of value change notifications!
        this->notify(NotificationTemperatureChanged, temperature.getValueAsJson());
    });
}

///////////////////////////////////////////////////////////////////////////////
// Requests:


///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentPreHeater::switchOn(const IFilamentPreHeater::Event&, const IFilamentPreHeater::State&)
{
    updateHeaterTemperature();

    if (!m_timerTemperatureObservation.start())
    {
        push(Event::ErrorOccurred);
        return;
    }

    push(Event::SwitchOnSucceeded);
}

void FilamentPreHeater::startHeating(const IFilamentPreHeater::Event& event,
                                     const IFilamentPreHeater::State&)
{
    m_propertyTemperature.setValue(getHeaterTemperature());

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

void FilamentPreHeater::checkTemperature(const Event&, const State& state)
{
    m_propertyTemperature.setValue(getHeaterTemperature());
    const auto temperatureState = getHeaterTemperatureState();

    if (TemperatureState::ErrorNoTemperature == temperatureState)
    {
        push(Event::ErrorOccurred);
        return;
    }

    if (State::HeatingOn == state && TemperatureState::AboveMaxTemperature == temperatureState)
    {
        LOG(debug) << "above max temperature: " << m_propertyTemperature.getValue() << " °C";
        push(Event::MaxTemperatureReached);
    }
    else if (State::HeatingOff == state &&
             TemperatureState::BelowMinTemperature == temperatureState)
    {
        LOG(debug) << "below min temperature: " << m_propertyTemperature.getValue() << " °C";
        push(Event::MinTemperatureReached);
    }
}

void FilamentPreHeater::switchOff(const IFilamentPreHeater::Event&,
                                  const IFilamentPreHeater::State&)
{
    m_timerTemperatureObservation.stop();

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
