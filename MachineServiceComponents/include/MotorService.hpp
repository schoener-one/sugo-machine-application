
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

#include "MachineConfig.hpp"
#include "ServiceLocator.hpp"

#include <atomic>
#include <string>

namespace sugo
{
/**
 * @class Class represents a heater
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
    explicit MotorService(const std::string& motorId, const ServiceLocator& serviceLocator);
    virtual ~MotorService() = default;

protected:
    void setMotorSpeed(unsigned motorSpeed);
    void setMotorOffsetSpeed(unsigned motorOffsetSpeed);
    bool startMotorRotation();
    bool resetMotor();
    void stopMotorRotation(bool immediately = false);

private:
    void setMotorSpeed();

    const std::string     m_motorId;
    const ServiceLocator& m_serviceLocator;
    unsigned              m_motorSpeed       = 0;
    unsigned              m_motorOffsetSpeed = 0;
};

}  // namespace sugo
