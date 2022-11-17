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
    const auto address =
        static_cast<uint8_t>(configuration.getOption(option::id::I2cAddress).get<unsigned>());
    m_maxSpeed = StepperMotor::StepperMotor::Speed(
        configuration.getOption(option::id::MaxSpeedRpm).get<unsigned>(), Unit::Rpm);
    m_initMaxSpeed = m_maxSpeed;
    LOG(debug) << getId() << "." << option::id::I2cAddress << ": " << address;
    LOG(debug) << getId() << "." << option::id::MaxSpeedRpm << ": " << m_maxSpeed.getValue();
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
    setMaxSpeed(m_initMaxSpeed);
    return true;
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
