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

#include "MachineServiceComponent/FilamentTensionSensorService.hpp"
#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/IHardwareAbstractionLayer.hpp"

using namespace sugo;
using namespace machine_service_component;

FilamentTensionSensorService::FilamentTensionSensorService(
    const hal::Identifier& lowTensionSensorId, const hal::Identifier& highTensionSensorId,
    const hal::Identifier& tensionOverloadSensorId, const common::ServiceLocator& serviceLocator)
    : HardwareService(serviceLocator.get<hal::IHardwareAbstractionLayer>()),
      m_lowTensionSensorObserver(
          getGpioPin(lowTensionSensorId),
          [this](const hal::IGpioPin::Event& event, const hal::Identifier& pinId) {
              this->handleFilamentTensionEvent(event, pinId);
          },
          std::chrono::milliseconds(serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigObservationTimeoutGpioPin)
                                        .get<unsigned>())),
      m_highTensionSensorObserver(
          getGpioPin(highTensionSensorId),
          [this](const hal::IGpioPin::Event& event, const hal::Identifier& pinId) {
              this->handleFilamentTensionEvent(event, pinId);
          },
          std::chrono::milliseconds(serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigObservationTimeoutGpioPin)
                                        .get<unsigned>())),
      m_tensionOverloadSensorObserver(
          getGpioPin(tensionOverloadSensorId),
          [this](const hal::IGpioPin::Event& event, const hal::Identifier& pinId) {
              this->handleFilamentTensionEvent(event, pinId);
          },
          std::chrono::milliseconds(serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigObservationTimeoutGpioPin)
                                        .get<unsigned>())),
      m_tensionEventRepeatTimer(
          std::chrono::milliseconds(serviceLocator.get<common::IConfiguration>()
                                        .getOption(id::ConfigObservationTimeoutTension)
                                        .get<unsigned>()),
          [this] { this->repeatFilamentTensionEvent(); }, lowTensionSensorId + "common::Timer")
{
}

bool FilamentTensionSensorService::startSensorObservation()
{
    assert(!isSensorObservationRunning());
    m_lastFilamentTensionEvent = FilamentTensionEvent::FilamentTensionNormal;

    if (!m_lowTensionSensorObserver.start())
    {
        LOG(error) << "Failed to start low tension sensor observation";
        return false;
    }

    if (!m_highTensionSensorObserver.start())
    {
        LOG(error) << "Failed to start high tension sensor observation";
        m_lowTensionSensorObserver.stop();
        return false;
    }

    if (!m_tensionOverloadSensorObserver.start())
    {
        LOG(error) << "Failed to start tension overload sensor observation";
        m_lowTensionSensorObserver.stop();
        m_highTensionSensorObserver.stop();
        return false;
    }

    return true;
}

void FilamentTensionSensorService::stopSensorObservation()
{
    m_lowTensionSensorObserver.stop();
    m_highTensionSensorObserver.stop();
    m_tensionOverloadSensorObserver.stop();
    m_tensionEventRepeatTimer.stop();
}

void FilamentTensionSensorService::repeatFilamentTensionEvent()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if ((m_lastFilamentTensionEvent == FilamentTensionLow) or
        (m_lastFilamentTensionEvent == FilamentTensionHigh))
    {
        // Repeat event call as long as the tension is not at normal level!
        onFilamentTensionEvent(m_lastFilamentTensionEvent);
    }
}

void FilamentTensionSensorService::handleFilamentTensionEvent(const hal::IGpioPin::Event& event,
                                                              const hal::Identifier&      pinId)
{
    FilamentTensionEvent currentEvent = FilamentTensionNormal;

    switch (event.type)
    {
        case hal::IGpioPin::EventType::RisingEdge:
            if (pinId == m_tensionOverloadSensorObserver.getId())
            {
                currentEvent = FilamentTensionOverload;
            }
            else if (pinId == m_lowTensionSensorObserver.getId())
            {
                currentEvent = FilamentTensionLow;
            }
            else if (pinId == m_highTensionSensorObserver.getId())
            {
                currentEvent = FilamentTensionHigh;
            }
            break;
        case hal::IGpioPin::EventType::FallingEdge:
            if (pinId == m_tensionOverloadSensorObserver.getId())
            {
                currentEvent = FilamentTensionHigh;
            }
            else
            {
                currentEvent = FilamentTensionNormal;
            }
            break;
        case hal::IGpioPin::EventType::Timeout:
            currentEvent = m_lastFilamentTensionEvent;
            break;
    }

    if (currentEvent != m_lastFilamentTensionEvent)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_lastFilamentTensionEvent = currentEvent;
        onFilamentTensionEvent(m_lastFilamentTensionEvent);

        if (currentEvent == FilamentTensionNormal)
        {
            m_tensionEventRepeatTimer.stop();
        }
        else if (!m_tensionEventRepeatTimer.isRunning())
        {
            m_tensionEventRepeatTimer.start();
        }
    }
}
