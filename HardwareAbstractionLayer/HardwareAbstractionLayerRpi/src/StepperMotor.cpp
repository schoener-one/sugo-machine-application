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

using namespace sugo::hal;

bool StepperMotor::init(const sugo::IConfiguration& configuration)
{
    m_i2cId    = configuration.getOption("i2c-id").get<unsigned>();
    m_maxSpeed = configuration.getOption("max-speed-rpm").get<unsigned>();
    LOG(debug) << getId() << ".i2c-id: " << m_i2cId;
    LOG(debug) << getId() << ".max-speed-rpm: " << m_maxSpeed;
    return true;
}

unsigned StepperMotor::rotate(unsigned steps, Direction direction, unsigned speed)
{
    (void)steps;
    (void)direction;
    (void)speed;
    return 0;
}

bool StepperMotor::stop()
{
    return false;
}

unsigned StepperMotor::getMicroStepCount() const
{
    return 0;
}

unsigned StepperMotor::getStepsPerRound() const
{
    return 0;
}

unsigned StepperMotor::getSpeed() const
{
    return 0;
}

unsigned StepperMotor::getMaxSpeed() const
{
    return 0;
}