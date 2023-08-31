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

#pragma once

#include <atomic>
#include <mutex>

#include "Common/IRunnable.hpp"
#include "Common/ServiceLocator.hpp"
#include "Common/Timer.hpp"
#include "HardwareAbstractionLayer/IHalObject.hpp"
#include "MachineServiceComponent/GpioPinEventObserver.hpp"
#include "MachineServiceComponent/HardwareService.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides a filament tension sensor control service.
class FilamentTensionSensorService : public HardwareService
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
    FilamentTensionSensorService(const hal::Identifier&        lowTensionSensorId,
                                 const hal::Identifier&        highTensionSensorId,
                                 const hal::Identifier&        tensionOverloadSensorId,
                                 const common::ServiceLocator& serviceLocator);

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

    GpioPinEventObserver m_lowTensionSensorObserver;   ///< Observer for low tension sensor.
    GpioPinEventObserver m_highTensionSensorObserver;  ///< Observer for high tension sensor.
    GpioPinEventObserver
                                      m_tensionOverloadSensorObserver;  ///< Observer for tension sensor overload.
    std::atomic<FilamentTensionEvent> m_lastFilamentTensionEvent =
        FilamentTensionEvent::FilamentTensionNormal;  ///< Last filament tension event.
    common::Timer m_tensionEventRepeatTimer;          ///< Timer to check tension events.
    std::mutex    m_mutex;                            ///< Mutex to avoid simultaneous access.
};
}  // namespace sugo::machine_service_component