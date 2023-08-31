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

#include "HardwareAbstractionLayer/StepperMotor.hpp"
#include "Common/IConfiguration.hpp"
#include "Common/Logger.hpp"

namespace
{
sugo::hal::IStepperMotor::Position s_curPosition = 0;
}
using namespace sugo::hal;

StepperMotor::~StepperMotor()
{
    finalize();
}

bool StepperMotor::init(const sugo::common::IConfiguration& configuration)
{
    const auto address =
        static_cast<uint8_t>(configuration.getOption(id::I2cAddress).get<unsigned>());
    m_maxSpeed =
        IStepperMotor::Speed(configuration.getOption(id::MaxSpeedRpm).get<unsigned>(), Unit::Rpm);
    m_direction = (configuration.getOption(id::Direction).get<std::string>() == "backward")
                      ? IStepperMotor::Direction::Backward
                      : IStepperMotor::Direction::Forward;
    LOG(debug) << getId() << "." << id::I2cAddress << ": " << address;
    LOG(debug) << getId() << "." << id::MaxSpeedRpm << ": " << m_maxSpeed;
    LOG(debug) << getId() << "." << id::Direction << ": " << m_direction;
    s_curPosition = 0;
    return true;
}

void StepperMotor::finalize()
{
}

bool StepperMotor::reset()
{
    if (getSpeed().getValue() != 0)
    {
        stop(true);
    }
    return true;
}

bool StepperMotor::rotateToPosition(Position position)
{
    s_curPosition += position;
    return true;
}

bool StepperMotor::rotate(Direction direction)
{
    m_direction = direction;
    return true;
}

bool StepperMotor::stop(bool stopImmediately)
{
    (void)stopImmediately;
    return true;
}

StepperMotor::Position StepperMotor::getPosition() const
{
    return s_curPosition;
}

StepperMotor::StepCount StepperMotor::getMicroStepCount() const
{
    static constexpr StepCount microStepCount = 1u;
    return microStepCount;
}

StepperMotor::StepCount StepperMotor::getStepsPerRound() const
{
    static constexpr StepCount stepsPerRound = 200u;
    return stepsPerRound;
}

StepperMotor::Speed StepperMotor::getSpeed() const
{
    return Speed(0u, Unit::Rpm);
}

bool StepperMotor::setSpeed(Speed speed)
{
    if (speed.getValue() > m_maxSpeed.getValue())
    {
        LOG(error) << getId() << ": Set target speed (" << speed << ") exceeds max speed ("
                   << m_maxSpeed << ")";
        return false;
    }

    m_speed = speed;
    return true;
}