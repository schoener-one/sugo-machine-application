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

#include <memory>
#include <string>

#include "Common/ServiceLocator.hpp"
#include "HardwareAbstractionLayer/IStepperMotor.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/HardwareService.hpp"

namespace sugo::machine_service_component
{
/// @brief Class provides control of the motor unit.
class MotorService : public HardwareService
{
public:
    /**
     * @brief Construct a new motor service object.
     *
     * @param motorId        Motor id of the motor to be used.
     * @param serviceLocator Service locator which contains the HAL interface.
     */
    MotorService(const hal::Identifier& motorId, const common::ServiceLocator& serviceLocator);

protected:
    /**
     * @brief Sets the motor max speed.
     * The value will only be set within the allowed range.
     *
     * @param motorSpeed Motor speed to set
     * @return true      If the speed value could be set successfully.
     * @return false     If the speed value could not be set successfully.
     */
    bool setMotorSpeed(unsigned motorSpeed);

    /**
     * @brief Sets the motor offset speed, which will be added to the current speed.
     * The value will only be set within the allowed range.
     *
     * @param motorSpeed Motor speed to set
     * @return true      If the offset speed value could be set successfully.
     * @return false     If the offset speed value could not be set successfully.
     */
    bool addMotorOffsetSpeed(int motorOffsetSpeed);

    /**
     * @brief Starts the motor rotation.
     *
     * @return true If the rotation could be started successfully.
     * @return false If the rotation could not be started successfully.
     */
    bool startMotorRotation();

    /**
     * @brief Resets the motor state.
     *
     * @return true If the rotation could be reset successfully.
     * @return false If the rotation could not be reset successfully.
     */
    bool resetMotor();

    /**
     * @brief Stops the motor rotation.
     *
     * @param immediately Indicates if the rotation should be stopped immediately.
     */
    void stopMotorRotation(bool immediately = false);

private:
    /**
     * @brief Sets the motor speed.
     *
     * @return true If the motor speed could be set.
     * @return false If the motor speed could not be set.
     */
    bool setMotorSpeed();

    std::shared_ptr<hal::IStepperMotor> m_stepperMotor;          ///< Stepper motor object.
    const unsigned                      m_maxMotorSpeed    = 0;  ///< Max motor speed.
    unsigned                            m_motorSpeed       = 0;  ///< Current set motor speed.
    int                                 m_motorOffsetSpeed = 0;  ///< Motor offset speed.
};

}  // namespace sugo::machine_service_component
