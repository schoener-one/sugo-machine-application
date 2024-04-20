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
    const hal::Identifier& tensionOverloadSensorId, const common::ServiceLocator& serviceLocator,
    FilamentTensionStateChangeHandler handler)
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
      m_filamentTensionEventHandler(std::move(handler))
{
}

bool FilamentTensionSensorService::startSensorObservation()
{
    assert(!isSensorObservationRunning());
    m_currentFilamentTensionState = FilamentTensionState::FilamentTensionNormal;

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
}

void FilamentTensionSensorService::handleFilamentTensionEvent(const hal::IGpioPin::Event& event,
                                                              const hal::Identifier&      pinId)
{
    FilamentTensionState newState = FilamentTensionNormal;

    switch (event.type)
    {
        case hal::IGpioPin::EventType::RisingEdge:
            if (pinId == m_tensionOverloadSensorObserver.getId())
            {
                newState = FilamentTensionOverload;
            }
            else if (pinId == m_lowTensionSensorObserver.getId())
            {
                newState = FilamentTensionLow;
            }
            else if (pinId == m_highTensionSensorObserver.getId())
            {
                newState = FilamentTensionHigh;
            }
            break;
        case hal::IGpioPin::EventType::FallingEdge:
            if (pinId == m_tensionOverloadSensorObserver.getId() &&
                m_currentFilamentTensionState == FilamentTensionOverload)
            {
                // In this case we're comming from an overloaded state back to tension high state!
                newState = FilamentTensionHigh;
            }
            else
            {
                newState = FilamentTensionNormal;
            }
            break;
        case hal::IGpioPin::EventType::Timeout:
            newState = m_currentFilamentTensionState;
            break;
    }

    if (newState != m_currentFilamentTensionState)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentFilamentTensionState = newState;
        m_filamentTensionEventHandler(newState);
    }
}
