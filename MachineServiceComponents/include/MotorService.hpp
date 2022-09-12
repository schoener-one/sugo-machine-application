
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

#include "ServiceLocator.hpp"
#include "Timer.hpp"
#include "MachineConfig.hpp"

#include <atomic>

namespace sugo
{
/**
 * @class Class represents a heater
 */
class MotorService
{
public:
    // Constructor / Destructor
    explicit MotorService(const ServiceLocator& serviceLocator)
        : m_serviceLocator(serviceLocator)
    {
    }
    virtual ~MotorService()
    {
    }

protected:
    void setMotorSpeed(unsigned motorSpeed);
    void setMotorOffsetSpeed(unsigned motorOffsetSpeed);
    bool startMotorRotation();
    bool resetMotor();
    void stopMotorRotation(bool immediately = false);
    
private:
    void setMotorSpeed();
    const ServiceLocator& m_serviceLocator;
    unsigned m_motorSpeed = 0;
    unsigned m_motorOffsetSpeed = 0;
};

}  // namespace sugo
