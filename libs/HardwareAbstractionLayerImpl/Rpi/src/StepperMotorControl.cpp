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

#include <cassert>

#include "HardwareAbstractionLayer/HalHelper.hpp"
#include "HardwareAbstractionLayer/I2cControl.hpp"
#include "HardwareAbstractionLayer/StepperMotor.hpp"
#include "HardwareAbstractionLayer/StepperMotorControl.hpp"

using namespace sugo::hal;

StepperMotorControl::~StepperMotorControl()
{
    finalize();
}

bool StepperMotorControl::init(const common::IConfiguration& configuration)
{
    assert(m_i2c == nullptr);

    auto deviceName = std::string("/dev/") + configuration.getOption("device").get<std::string>();
    LOG(debug) << getId() << ": Open device '" << deviceName << "'";
    m_i2c = new I2cControl();

    if (!m_i2c->init(deviceName))
    {
        LOG(error) << getId() << ": Failed to initialize I2C bus";
        finalize();
        return false;
    }

    return initEnabledSubComponents<IStepperMotor, StepperMotor>(
        configuration, "motor", m_stepperMotorMap, *m_i2c, m_ioErr, m_ioRst);
}

void StepperMotorControl::finalize()
{
    if (m_i2c != nullptr)
    {
        delete m_i2c;
        m_i2c = nullptr;
    }
}

void StepperMotorControl::reset()
{
    // FIXME
    LOG(warning) << getId() << ": Not implemented yet!";
}
