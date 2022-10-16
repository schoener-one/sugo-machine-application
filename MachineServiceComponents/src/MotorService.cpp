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

using namespace sugo;

MotorService::MotorService(const std::string&    motorId,
                                    const ServiceLocator& serviceLocator)
    : m_motorId(motorId), m_serviceLocator(serviceLocator)
{
}

void MotorService::setMotorSpeed(unsigned motorSpeed)
{
    m_motorSpeed = motorSpeed;
    setMotorSpeed();
}

void MotorService::setMotorSpeed()
{
    auto& stepperMotor =
        getStepperMotor(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), m_motorId);
    const auto motorSpeed = m_motorSpeed + m_motorOffsetSpeed;
    LOG(debug) << "Setting motor speed: " << motorSpeed;
    stepperMotor->setMaxSpeed(hal::IStepperMotor::Speed(motorSpeed, hal::Unit::Rpm));
}

void MotorService::setMotorOffsetSpeed(unsigned motorOffsetSpeed)
{
    m_motorOffsetSpeed = motorOffsetSpeed;
    setMotorSpeed();
}

bool MotorService::startMotorRotation()
{
    auto& stepperMotor =
        getStepperMotor(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), m_motorId);

    if (!stepperMotor->rotate(hal::IStepperMotor::Direction::Forward))
    {
        LOG(error) << "Failed to start motor rotation";
        return false;
    }

    return true;
}

bool MotorService::resetMotor()
{
    auto& stepperMotor =
        getStepperMotor(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), m_motorId);

    if (!stepperMotor->reset())
    {
        LOG(error) << "Failed to reset motor state";
        return false;
    }

    return true;
}

void MotorService::stopMotorRotation(bool immediately)
{
    auto& stepperMotor =
        getStepperMotor(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(), m_motorId);

    if (!stepperMotor->stop(immediately))
    {
        LOG(error) << "Failed to stop motor" << ((immediately) ? " immediately" : "");
    }
}