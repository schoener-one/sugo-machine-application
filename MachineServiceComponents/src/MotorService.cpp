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

namespace
{
constexpr unsigned HalfSpeedDivider = 2u;
}

using namespace sugo;

MotorService::MotorService(const hal::Identifier& motorId, const ServiceLocator& serviceLocator)
    : m_stepperMotor(
          getStepperMotor(serviceLocator.get<hal::IHardwareAbstractionLayer>(), motorId)),
      m_maxMotorSpeed(m_stepperMotor->getMaxSpeed().getValue()),
      m_motorSpeed(0)
{
}

bool MotorService::setMotorSpeed(unsigned motorSpeed)
{
    m_motorSpeed = motorSpeed;

    if (m_motorSpeed > m_maxMotorSpeed)
    {
        m_motorSpeed = m_maxMotorSpeed;
    }

    return setMotorSpeed();
}

bool MotorService::setMotorSpeed()
{
    assert(static_cast<unsigned>(std::abs(m_motorOffsetSpeed)) <= m_motorSpeed);
    const int motorSpeed = static_cast<int>(m_motorSpeed) + m_motorOffsetSpeed;
    LOG(debug) << "Setting motor speed: " << motorSpeed;
    return m_stepperMotor->setSpeed(
        hal::IStepperMotor::Speed(static_cast<unsigned>(motorSpeed), hal::Unit::Rpm));
}

bool MotorService::addMotorOffsetSpeed(int motorOffsetSpeed)
{
    int resultingSpeed =
        std::clamp(static_cast<int>(m_motorSpeed) + m_motorOffsetSpeed + motorOffsetSpeed, 0,
                   static_cast<int>(m_maxMotorSpeed));
    m_motorOffsetSpeed = resultingSpeed - static_cast<int>(m_motorSpeed);
    return setMotorSpeed();
}

bool MotorService::startMotorRotation()
{
    setMotorSpeed();

    if (!m_stepperMotor->rotate())
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
    m_motorSpeed       = 0;

    return true;
}

void MotorService::stopMotorRotation(bool immediately)
{
    if (!m_stepperMotor->stop(immediately))
    {
        LOG(error) << "Failed to stop motor" << ((immediately) ? " immediately" : "");
    }
}