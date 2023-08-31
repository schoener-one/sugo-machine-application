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

#include "HardwareAbstractionLayer/IGpioPin.hpp"
#include "HardwareAbstractionLayer/IStepperMotor.hpp"

namespace sugo::hal
{
class TicController;
class I2cControl;

/// @brief Class represents an ADC input
class StepperMotor : public IStepperMotor
{
public:
    StepperMotor(const Identifier& id, I2cControl& i2cControl, IGpioPin& ioErr, IGpioPin& ioRst)
        : IStepperMotor(id), m_i2cControl(i2cControl), m_ioErr(ioErr), m_ioRst(ioRst)
    {
    }
    ~StepperMotor() override;

    bool init(const common::IConfiguration& configuration) override;
    void finalize();

    bool reset() override;
    bool rotateToPosition(Position position) override;
    bool rotate(Direction direction) override;

    bool rotate() override
    {
        return rotate(m_direction);
    }

    bool      stop(bool stopImmediately = false) override;
    Position  getPosition() const override;
    StepCount getMicroStepCount() const override;
    StepCount getStepsPerRound() const override;
    Speed     getSpeed() const override;

    Speed getMaxSpeed() const override
    {
        return m_maxSpeed;
    }

    void setMaxSpeed(Speed maxSpeed) override
    {
        m_maxSpeed = maxSpeed;
    }

    bool setSpeed(Speed speed) override;

private:
    bool prepareForMotion();
    bool waitAndShutdown(std::chrono::milliseconds remainingMotionTime,
                         bool                      stopImmediately = false);

    I2cControl&    m_i2cControl;
    IGpioPin&      m_ioErr;
    IGpioPin&      m_ioRst;
    TicController* m_controller = nullptr;
    Speed          m_maxSpeed   = Speed(0, Unit::Rpm);
    Speed          m_speed      = Speed(0, Unit::Rpm);
    Direction      m_direction  = Direction::Forward;
};

}  // namespace sugo::hal
