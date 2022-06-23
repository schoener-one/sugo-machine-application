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
#include "StepperMotor.hpp"

namespace sugo::hal
{
class I2cControl
{
public:
    I2cControl() = default;
};
}  // namespace sugo::hal

using namespace sugo::hal;

StepperMotorControl::~StepperMotorControl()
{
    finalize();
}

bool StepperMotorControl::init(const IConfiguration& configuration)
{
    static I2cControl s_i2cControlDummy;

    auto deviceName = configuration.getOption("device").get<std::string>();
    LOG(debug) << getId() << ": Open device '" << deviceName << "'";

    return initEnabledSubComponents<IStepperMotor, StepperMotor>(
        configuration, "motor", m_stepperMotorMap, s_i2cControlDummy, m_ioErr, m_ioRst);
}

void StepperMotorControl::finalize()
{
}

void StepperMotorControl::reset()
{
    // FIXME
    LOG(warning) << getId() << ": Not implemented yet!";
}
