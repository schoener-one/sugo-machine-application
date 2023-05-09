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
    m_maxSpeed = IStepperMotor::Speed(
        configuration.getOption(option::id::MaxSpeedRpm).get<unsigned>(), Unit::Rpm);
    m_direction = (configuration.getOption(option::id::Direction).get<std::string>() == "backward")
                      ? IStepperMotor::Direction::Backward
                      : IStepperMotor::Direction::Forward;
    LOG(debug) << getId() << "." << option::id::I2cAddress << ": " << address;
    LOG(debug) << getId() << "." << option::id::MaxSpeedRpm << ": " << m_maxSpeed;
    LOG(debug) << getId() << "." << option::id::Direction << ": " << m_direction;
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