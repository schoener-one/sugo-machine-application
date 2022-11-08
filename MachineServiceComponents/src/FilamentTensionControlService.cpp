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
    : m_lowTensionSensorId(
          getGpioPin(serviceLocator.get<hal::IHardwareAbstractionLayer>(), lowTensionSensorId),
          [this](const hal::IGpioPin::Event& event, const hal::Identifier& pinId) {
              this->filterFilamentTensionEvent(event, pinId);
          },
          config::MaxGpioPinObservationTimeout),
      m_highTensionSensorId(
          getGpioPin(serviceLocator.get<hal::IHardwareAbstractionLayer>(), highTensionSensorId),
          [this](const hal::IGpioPin::Event& event, const hal::Identifier& pinId) {
              this->filterFilamentTensionEvent(event, pinId);
          },
          config::MaxGpioPinObservationTimeout)
{
}

bool FilamentTensionControlService::startSensorObservation()
{
    assert(!isSensorObservationRunning());
    m_lastFilamentTensionEvent = FilamentTensionEvent::FilamentTensionNormal;

    if (!m_lowTensionSensorId.start())
    {
        LOG(error) << "Failed to start low tension sensor observation";
        return false;
    }

    if (!m_highTensionSensorId.start())
    {
        LOG(error) << "Failed to start high tension sensor observation";
        m_lowTensionSensorId.stop();
        return false;
    }

    return true;
}

void FilamentTensionControlService::stopSensorObservation()
{
    m_lowTensionSensorId.stop();
    m_highTensionSensorId.stop();
}

void FilamentTensionControlService::filterFilamentTensionEvent(
    const hal::IGpioPin::Event& gpioEvent, const hal::Identifier& pinId)
{
    FilamentTensionEvent nextEvent = FilamentTensionNormal;

    switch (gpioEvent.type)
    {
        case hal::IGpioPin::EventType::RisingEdge:
            if (pinId == m_lowTensionSensorId.getId())
            {
                nextEvent = FilamentTensionLow;
            }
            else
            {
                nextEvent = FilamentTensionHigh;
            }
            break;
        case hal::IGpioPin::EventType::FallingEdge:
            nextEvent = FilamentTensionNormal;
            break;
        case hal::IGpioPin::EventType::Timeout:
            nextEvent = m_lastFilamentTensionEvent;
            break;
    }

    if ((nextEvent != m_lastFilamentTensionEvent) or (nextEvent == FilamentTensionLow) or
        (nextEvent == FilamentTensionHigh))
    {
        m_lastFilamentTensionEvent = nextEvent;
        onFilamentTensionEvent(m_lastFilamentTensionEvent);
    }
}