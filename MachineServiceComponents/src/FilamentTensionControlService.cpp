///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-11-01
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentTensionControlService.hpp"
#include "HardwareAbstractionLayerHelper.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "Logger.hpp"

using namespace sugo;

FilamentTensionControlService::FilamentTensionControlService(
    const hal::Identifier& lowTensionSensorId, const hal::Identifier& highTensionSensorId,
    const ServiceLocator& serviceLocator)
    : m_lowTensionSensorObserver(
          getGpioPin(serviceLocator.get<hal::IHardwareAbstractionLayer>(), lowTensionSensorId),
          [this](const hal::IGpioPin::Event& event, const hal::Identifier& pinId) {
              this->filterFilamentTensionEvent(event, pinId);
          },
          std::chrono::milliseconds(serviceLocator.get<IConfiguration>()
                                        .getOption(id::ObservationTimeoutGpioPin)
                                        .get<unsigned>())),
      m_highTensionSensorObserver(
          getGpioPin(serviceLocator.get<hal::IHardwareAbstractionLayer>(), highTensionSensorId),
          [this](const hal::IGpioPin::Event& event, const hal::Identifier& pinId) {
              this->filterFilamentTensionEvent(event, pinId);
          },
          std::chrono::milliseconds(serviceLocator.get<IConfiguration>()
                                        .getOption(id::ObservationTimeoutGpioPin)
                                        .get<unsigned>())),
      m_tensionEventRepeatTimer(
          std::chrono::milliseconds(serviceLocator.get<IConfiguration>()
                                        .getOption(id::ObservationTimeoutTension)
                                        .get<unsigned>()),
          [this] { this->repeatFilamentTensionEvent(); }, lowTensionSensorId + "Timer")
{
}

bool FilamentTensionControlService::startSensorObservation()
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

    return true;
}

void FilamentTensionControlService::stopSensorObservation()
{
    m_lowTensionSensorObserver.stop();
    m_highTensionSensorObserver.stop();
    m_tensionEventRepeatTimer.stop();
}

void FilamentTensionControlService::repeatFilamentTensionEvent()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_lastFilamentTensionEvent != FilamentTensionNormal)
    {
        // Repeat event call as long as the tension is not at normal level!
        onFilamentTensionEvent(m_lastFilamentTensionEvent);
    }
}

void FilamentTensionControlService::filterFilamentTensionEvent(
    const hal::IGpioPin::Event& gpioEvent, const hal::Identifier& pinId)
{
    FilamentTensionEvent currentEvent = FilamentTensionNormal;

    switch (gpioEvent.type)
    {
        case hal::IGpioPin::EventType::RisingEdge:
            if (pinId == m_lowTensionSensorObserver.getId())
            {
                currentEvent = FilamentTensionLow;
            }
            else
            {
                currentEvent = FilamentTensionHigh;
            }
            break;
        case hal::IGpioPin::EventType::FallingEdge:
            currentEvent = FilamentTensionNormal;
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
        else
        {
            m_tensionEventRepeatTimer.start();
        }
    }
}