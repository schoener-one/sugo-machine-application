
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-15
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IStepperMotor.hpp"
#include "MachineConfig.hpp"
#include "ServiceLocator.hpp"

#include <memory>
#include <string>

namespace sugo
{
/**
 * @class Class provides a motor control service
 *
 */
class MotorService
{
public:
    /**
     * @brief Construct a new motor service object.
     *
     * @param motorId        Motor id of the motor to be used.
     * @param serviceLocator Service locator which contains the HAL interface.
     */
    explicit MotorService(const hal::Identifier& motorId, const ServiceLocator& serviceLocator);

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
    bool setMotorOffsetSpeed(int motorOffsetSpeed);
    bool startMotorRotation();
    bool resetMotor();
    void stopMotorRotation(bool immediately = false);

private:
    void setMotorSpeed();

    std::shared_ptr<hal::IStepperMotor> m_stepperMotor;
    unsigned                            m_motorSpeed       = 0;
    unsigned                            m_motorOffsetSpeed = 0;
};

}  // namespace sugo
