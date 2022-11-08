///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-11-01
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GpioPinEventObserver.hpp"
#include "IHalObject.hpp"
#include "IRunnable.hpp"
#include "ServiceLocator.hpp"

#include <atomic>

namespace sugo
{
/**
 * @brief Class provides a filament tension sensor control service.
 *
 */
class FilamentTensionControlService
{
public:
    FilamentTensionControlService(const hal::Identifier& lowTensionSensorId,
                                  const hal::Identifier& highTensionSensorId,
                                  const ServiceLocator&  serviceLocator);
    ~FilamentTensionControlService() = default;

protected:
    /**
     * @brief Filament tension event.
     *
     */
    enum FilamentTensionEvent
    {
        FilamentTensionLow,
        FilamentTensionNormal,
        FilamentTensionHigh
    };

    /**
     * @brief Starts the tension sensor observation.
     *
     * @return true  If the sensor observation could be started successfully.
     * @return false If the sensor observation could not be started successfully.
     */
    bool startSensorObservation();

    /**
     * @brief Indicates if the sensor observation is still running.
     *
     * @return true  If the sensor observation is running.
     * @return false If the sensor observation is not running.
     */
    bool isSensorObservationRunning() const
    {
        return m_lowTensionSensorId.isRunning() && m_highTensionSensorId.isRunning();
    }

    /**
     * @brief Stops the filament tension sensor observation.
     *
     */
    void stopSensorObservation();

    /**
     * @brief Called if a filament tension event occurred.
     *
     * @param event Filament tension event.
     */
    virtual void onFilamentTensionEvent(FilamentTensionEvent event) = 0;

private:
    void filterFilamentTensionEvent(const hal::IGpioPin::Event& gpioEvent,
                                    const hal::Identifier&      pinId);

    GpioPinEventObserver              m_lowTensionSensorId;
    GpioPinEventObserver              m_highTensionSensorId;
    std::atomic<FilamentTensionEvent> m_lastFilamentTensionEvent =
        FilamentTensionEvent::FilamentTensionNormal;
};
}  // namespace sugo