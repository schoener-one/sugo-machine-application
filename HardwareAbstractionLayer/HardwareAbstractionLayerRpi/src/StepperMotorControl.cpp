///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#include "StepperMotorControl.hpp"
#include "HalHelper.hpp"
#include "I2cControl.hpp"
#include "StepperMotor.hpp"

#include <cassert>

using namespace sugo::hal;

StepperMotorControl::~StepperMotorControl()
{
    finalize();
}

bool StepperMotorControl::init(const IConfiguration& configuration)
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
