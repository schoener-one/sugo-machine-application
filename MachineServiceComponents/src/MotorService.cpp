///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-15
 */
///////////////////////////////////////////////////////////////////////////////

#include "MotorService.hpp"
#include "HardwareAbstractionLayerHelper.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "IStepperMotor.hpp"
#include "Logger.hpp"
#include "MachineConfig.hpp"

#include <algorithm>

using namespace sugo;

MotorService::MotorService(const hal::Identifier& motorId, const ServiceLocator& serviceLocator)
    : m_stepperMotor(getStepperMotor(serviceLocator.get<hal::IHardwareAbstractionLayer>(), motorId))
{
}

bool MotorService::setMotorSpeed(unsigned motorSpeed)
{
    bool valid = true;

    if (motorSpeed <= config::MaxMotorSpeed)
    {
        m_motorSpeed = motorSpeed;
    }
    else
    {
        m_motorSpeed = config::MaxMotorSpeed;
        valid        = false;
    }

    setMotorSpeed();
    return valid;
}

void MotorService::setMotorSpeed()
{
    const auto motorSpeed = m_motorSpeed + m_motorOffsetSpeed;
    LOG(debug) << "Setting motor speed: " << motorSpeed;
    m_stepperMotor->setMaxSpeed(hal::IStepperMotor::Speed(motorSpeed, hal::Unit::Rpm));
}

bool MotorService::setMotorOffsetSpeed(int motorOffsetSpeed)
{
    int  resultingSpeed = static_cast<int>(m_motorSpeed) + motorOffsetSpeed;
    bool valid          = true;

    if (resultingSpeed >= static_cast<int>(config::MinMotorSpeed) and
        (resultingSpeed <= static_cast<int>(config::MaxMotorSpeed)))
    {
        m_motorOffsetSpeed = motorOffsetSpeed;
    }
    else
    {
        resultingSpeed     = std::clamp(motorOffsetSpeed, static_cast<int>(config::MinMotorSpeed),
                                    static_cast<int>(config::MaxMotorSpeed));
        m_motorOffsetSpeed = static_cast<int>(m_motorSpeed) - resultingSpeed;
        valid              = false;
    }

    setMotorSpeed();
    return valid;
}

bool MotorService::startMotorRotation()
{
    setMotorSpeed();

    if (!m_stepperMotor->rotate(hal::IStepperMotor::Direction::Forward))
    {
        LOG(error) << "Failed to start motor rotation";
        return false;
    }

    return true;
}

bool MotorService::resetMotor()
{
    if (!m_stepperMotor->reset())
    {
        LOG(error) << "Failed to reset motor state";
        return false;
    }

    m_motorOffsetSpeed = 0;
    m_motorSpeed       = m_stepperMotor->getMaxSpeed().getValue();

    return true;
}

void MotorService::stopMotorRotation(bool immediately)
{
    if (!m_stepperMotor->stop(immediately))
    {
        LOG(error) << "Failed to stop motor" << ((immediately) ? " immediately" : "");
    }
}