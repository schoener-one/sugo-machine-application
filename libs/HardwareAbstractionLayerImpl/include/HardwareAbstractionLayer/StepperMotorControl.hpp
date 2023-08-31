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

#include "Common/IConfiguration.hpp"
#include "HardwareAbstractionLayer/IGpioPin.hpp"
#include "HardwareAbstractionLayer/IStepperMotorControl.hpp"

namespace sugo::hal
{
class I2cControl;

/// @brief Class for contolling stepper motors.
class StepperMotorControl : public IStepperMotorControl
{
public:
    StepperMotorControl(const Identifier& id, IGpioPin& ioErr, IGpioPin& ioRst)
        : IStepperMotorControl(id), m_ioErr(ioErr), m_ioRst(ioRst)
    {
    }
    ~StepperMotorControl() override;

    bool init(const common::IConfiguration& configuration) override;

    void finalize();

    void reset() override;

    const StepperMotorMap& getStepperMotorMap() override
    {
        return m_stepperMotorMap;
    }

private:
    IGpioPin&       m_ioErr;
    IGpioPin&       m_ioRst;
    I2cControl*     m_i2c = nullptr;
    StepperMotorMap m_stepperMotorMap;
};

}  // namespace sugo::hal
