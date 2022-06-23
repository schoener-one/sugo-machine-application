///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-11
 */
///////////////////////////////////////////////////////////////////////////////

#include "StepperMotor.hpp"
#include "IConfiguration.hpp"
#include "Logger.hpp"

namespace
{
    sugo::hal::IStepperMotor::Position s_curPosition = 0;
}
using namespace sugo::hal;

StepperMotor::~StepperMotor()
{
    finalize();
}

bool StepperMotor::init(const sugo::IConfiguration& configuration)
{
    const uint8_t address =
        static_cast<uint8_t>(configuration.getOption("i2c-address").get<unsigned>());
    m_maxSpeed = StepperMotor::StepperMotor::Speed(
        configuration.getOption("max-speed-rpm").get<unsigned>(), Unit::Rpm);
    LOG(debug) << getId() << ".i2c-address: " << address;
    LOG(debug) << getId() << ".max-speed-rpm: " << m_maxSpeed.getValue();
    s_curPosition = 0;
    return true;
}

void StepperMotor::finalize()
{
}

bool StepperMotor::rotateToPosition(Position position)
{
    s_curPosition += position;
    return true;
}

bool StepperMotor::rotate(Direction direction)
{
    (void)direction;
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
    return 1u;
}

StepperMotor::StepCount StepperMotor::getStepsPerRound() const
{
    return 200u;
}

StepperMotor::Speed StepperMotor::getSpeed() const
{
    return Speed(0u, Unit::Rpm);
}
