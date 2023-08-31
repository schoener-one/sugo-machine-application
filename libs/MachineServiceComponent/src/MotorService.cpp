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

#include <algorithm>

#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/IHardwareAbstractionLayer.hpp"
#include "HardwareAbstractionLayer/IStepperMotor.hpp"
#include "MachineServiceComponent/Configuration.hpp"
#include "MachineServiceComponent/MotorService.hpp"

namespace
{
constexpr unsigned HalfSpeedDivider = 2u;
}

using namespace sugo;
using namespace sugo::machine_service_component;

MotorService::MotorService(const hal::Identifier&        motorId,
                           const common::ServiceLocator& serviceLocator)
    : HardwareService(serviceLocator.get<hal::IHardwareAbstractionLayer>()),
      m_stepperMotor(getStepperMotor(motorId)),
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
    assert((static_cast<int>(m_motorSpeed) + m_motorOffsetSpeed) <=
           static_cast<int>(m_maxMotorSpeed));
    const int motorSpeed = static_cast<int>(m_motorSpeed) + m_motorOffsetSpeed;
    LOG(debug) << "Setting motor speed: " << motorSpeed;
    return m_stepperMotor->setSpeed(
        hal::IStepperMotor::Speed(static_cast<unsigned>(motorSpeed), hal::Unit::Rpm));
}

// FIXME remove offset-speed, just use one speed!
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
    LOG(debug) << "Motor stop finished";
}