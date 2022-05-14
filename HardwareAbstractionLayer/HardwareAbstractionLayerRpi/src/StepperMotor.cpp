///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-05-14
 */
///////////////////////////////////////////////////////////////////////////////

#include "StepperMotor.hpp"
#include "IConfiguration.hpp"
#include "Logger.hpp"
#include "TicController.hpp"

#include <cassert>

namespace
{
constexpr unsigned s_microStepsPerStep  = 8u;
constexpr unsigned s_fullStepsPerRound  = 200u;
constexpr unsigned s_microStepsPerRound = s_fullStepsPerRound * s_microStepsPerStep;
const unsigned     s_secondsPerMinute   = 60u;

constexpr unsigned rpmToMicrostepsPer10kSeconds(unsigned speedRpm)
{
    return (speedRpm * s_microStepsPerRound * 10000u) / s_secondsPerMinute;
}
}  // namespace

using namespace sugo::hal;

StepperMotor::~StepperMotor()
{
    finalize();
}

bool StepperMotor::init(const sugo::IConfiguration& configuration)
{
    assert(m_controller == nullptr);

    const I2cControl::Address address =
        static_cast<I2cControl::Address>(configuration.getOption("i2c-address").get<unsigned>());
    m_maxSpeed = StepperMotor::Speed(configuration.getOption("max-speed-rpm").get<unsigned>(), Unit::Rpm);
    LOG(debug) << getId() << ".i2c-address: " << address;
    LOG(debug) << getId() << ".max-speed-rpm: " << m_maxSpeed;

    m_controller = new TicController(address, m_i2cControl, m_ioErr, m_ioRst);
    if (!m_controller->init())
    {
        LOG(error) << getId() << ": Failed to initialize motor control";
        finalize();
        return false;
    }

    if (!m_controller->setStepMode(TicController::StepMode::Step1_8))
    {
        LOG(error) << getId() << ": Failed to set step mode";
        finalize();
        return false;
    }

    if (!stop())
    {
        LOG(error) << getId() << ": Failed to stop motor";
        finalize();
        return false;
    }
    return true;
}

void StepperMotor::finalize()
{
    if (m_controller != nullptr)
    {
        delete m_controller;
        m_controller = nullptr;
    }
}

bool StepperMotor::rotateToPosition(Position position)
{
    assert(m_controller != nullptr);

    if (!m_controller->energize())
    {
        LOG(error) << getId() << ": Failed to energize motor";
        stop();
        return false;
    }

    // Set max velocity
    const unsigned maxSpeed = rpmToMicrostepsPer10kSeconds(m_maxSpeed.getValue());

    if (!m_controller->setMaxSpeed(maxSpeed))
    {
        LOG(error) << getId() << ": Failed to set max speed (" << m_maxSpeed.getValue() << "/"
                   << maxSpeed << ")";
        return false;
    }

    // Rotate to position
    //const Position stepsToMove = position - m_curPosition;
    if (!m_controller->setTargetPosition(position))
    {
        LOG(error) << getId() << ": Failed to reach motor position (" << position << ")";
        return false;
    }

    // Wait until position is reached
    return true;
}

bool StepperMotor::rotate(Direction direction)
{
    assert(m_controller != nullptr);

    if (!m_controller->energize())
    {
        LOG(error) << getId() << ": Failed to energize motor";
        stop();
        return false;
    }

    // Set max velocity
    const unsigned maxSpeed = rpmToMicrostepsPer10kSeconds(m_maxSpeed.getValue());

    if (!m_controller->setMaxSpeed(maxSpeed))
    {
        LOG(error) << getId() << ": Failed to set max speed (" << m_maxSpeed.getValue() << "/"
                   << maxSpeed << ")";
        return false;
    }

    const int32_t velocity =
        static_cast<int32_t>(maxSpeed) * ((direction == Direction::Forward) ? 1 : -1);

    if (!m_controller->setTargetVelocity(velocity))
    {
        LOG(error) << getId() << ": Failed to set target velocity";
        return false;
    }
    return true;
}

bool StepperMotor::stop()
{
    assert(m_controller != nullptr);

    bool success  = m_controller->haltAndSetPosition(0);
    success       = m_controller->deEnergize() && success;
    m_curPosition = 0;
    return success;
}

unsigned StepperMotor::getMicroStepCount() const
{
    return s_microStepsPerStep;
}

unsigned StepperMotor::getStepsPerRound() const
{
    return s_microStepsPerRound;
}

StepperMotor::Speed StepperMotor::getSpeed() const
{
    return Speed(0, Unit::Rpm);
}
