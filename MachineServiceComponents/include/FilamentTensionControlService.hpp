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
#include "Timer.hpp"

#include <atomic>
#include <mutex>

namespace sugo
{
/**
 * @brief Class provides a filament tension sensor control service.
 *
 */
class FilamentTensionControlService
{
public:
    /**
     * @brief Construct a new filament tension control service object.
     *
     * @param lowTensionSensorId       Id of the low tension sensor.
     * @param highTensionSensorId      Id of the hight tension sensor.
     * @param tensionOverloadSensorId  Id of the tension overload sensor.
     * @param serviceLocator           Service locator reference.
     */
    FilamentTensionControlService(const hal::Identifier& lowTensionSensorId,
                                  const hal::Identifier& highTensionSensorId,
                                  const hal::Identifier& tensionOverloadSensorId,
                                  const ServiceLocator&  serviceLocator);

protected:
    /**
     * @brief Filament tension event.
     *
     */
    enum FilamentTensionEvent
    {
        FilamentTensionLow,
        FilamentTensionNormal,
        FilamentTensionHigh,
        FilamentTensionOverload
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
        return m_lowTensionSensorObserver.isRunning() && m_highTensionSensorObserver.isRunning();
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
    void repeatFilamentTensionEvent();
    void handleFilamentTensionEvent(const hal::IGpioPin::Event& gpioEvent,
                                    const hal::Identifier&      pinId);

    GpioPinEventObserver              m_lowTensionSensorObserver;
    GpioPinEventObserver              m_highTensionSensorObserver;
    GpioPinEventObserver              m_tensionOverloadSensorObserver;
    std::atomic<FilamentTensionEvent> m_lastFilamentTensionEvent =
        FilamentTensionEvent::FilamentTensionNormal;
    Timer      m_tensionEventRepeatTimer;
    std::mutex m_mutex;
};
}  // namespace sugo