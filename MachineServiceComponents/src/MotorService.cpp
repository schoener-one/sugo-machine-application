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
#include "MachineConfig.hpp"
#include "Logger.hpp"

using namespace sugo;

void MotorService::setMotorSpeed(unsigned motorSpeed)
{
    m_motorSpeed = motorSpeed;
    setMotorSpeed();
}

void MotorService::setMotorSpeed()
{
    auto& stepperMotor    = getStepperMotor(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(),
                                         config::StepperMotorCoilerId);
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
    auto& stepperMotor = getStepperMotor(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(),
                                         config::StepperMotorCoilerId);
    return stepperMotor->rotate(hal::IStepperMotor::Direction::Forward);
}

bool MotorService::resetMotor()
{
    auto& stepperMotor = getStepperMotor(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(),
                                         config::StepperMotorCoilerId);
    if (!stepperMotor->reset())
    {
        return false;
    }
    return true;
}

void MotorService::stopMotorRotation(bool immediately)
{
    auto& stepperMotor = getStepperMotor(m_serviceLocator.get<hal::IHardwareAbstractionLayer>(),
                                         config::StepperMotorCoilerId);
    (void)stepperMotor->stop(immediately);
}