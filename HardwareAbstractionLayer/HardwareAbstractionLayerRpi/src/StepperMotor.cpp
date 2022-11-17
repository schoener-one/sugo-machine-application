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
#include "HalHelper.hpp"
#include "IConfiguration.hpp"
#include "Logger.hpp"
#include "TicController.hpp"

#include <cassert>
#include <cmath>
#include <thread>

namespace
{
constexpr int                                MicroStepsPerStep = 8;  ///< Step1_8
constexpr sugo::hal::TicController::StepMode StepMode = sugo::hal::TicController::StepMode::Step1_8;
constexpr int                                FullStepsPerRound = 200;
constexpr int                       MicroStepsPerRound    = FullStepsPerRound * MicroStepsPerStep;
constexpr int                       SecondsPerMinute      = 60u;
constexpr unsigned                  MillisecondsPerMinute = SecondsPerMinute * 1000u;
constexpr std::chrono::milliseconds MinWaitTime(2u);
constexpr std::chrono::milliseconds MaxWaitTime(500u);
constexpr uint16_t                  CurrentLimit = 1500u;

constexpr int rpmToMicrostepsPer10kSeconds(int speedRpm)
{
    return (speedRpm * MicroStepsPerRound * 10000) / SecondsPerMinute;
}

constexpr int microstepsPer10kSecondsToRpm(int microstepsPer10kSeconds)
{
    return (microstepsPer10kSeconds * SecondsPerMinute) / (MicroStepsPerRound * 10000);
}

/// Calculate the minimum motion time depending on speed and distance (microsteps)!
constexpr std::chrono::milliseconds calculateMotionTime(unsigned speedRpm, unsigned microsteps)
{
    const unsigned divider = MicroStepsPerRound * speedRpm;
    return std::chrono::milliseconds((microsteps * MillisecondsPerMinute) / divider);
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

    const I2cControl::Address address = static_cast<I2cControl::Address>(
        configuration.getOption(option::id::I2cAddress).get<unsigned>());
    m_maxSpeed = StepperMotor::Speed(
        configuration.getOption(option::id::MaxSpeedRpm).get<unsigned>(), Unit::Rpm);
    m_initMaxSpeed = m_maxSpeed;
    LOG(debug) << getId() << ".i2c-address: " << address;
    LOG(debug) << getId() << ".max-speed-rpm: " << m_maxSpeed;

    m_controller = new TicController(address, m_i2cControl, m_ioErr, m_ioRst);
    if (!m_controller->init())
    {
        LOG(error) << getId() << ": Failed to initialize motor control";
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

bool StepperMotor::reset()
{
    if (getSpeed().getValue() != 0)
    {
        stop(true);
    }
    setMaxSpeed(m_initMaxSpeed);
    return true;
}

StepperMotor::Position StepperMotor::getPosition() const
{
    assert(m_controller != nullptr);

    TicController::State state;
    if (!m_controller->getState(state))
    {
        LOG(error) << getId() << ": Failed to get current position";
        return 0;
    }

    return state.currentPosition;
}

bool StepperMotor::prepareForMotion()
{
    assert(m_controller != nullptr);

    // Check if pre state is ok
    TicController::State state;
    if (!m_controller->getState(state))
    {
        LOG(error) << getId() << ": Failed to get motion state";
        return false;
    }

    if (state.currentVelocity != 0)
    {
        LOG(error) << getId() << ": Motor is still moving";
        return false;
    }

    // (Re-)configure the correct motor control settings
    if (!m_controller->setStepMode(StepMode))
    {
        LOG(error) << getId() << ": Failed to set step mode";
        finalize();
        return false;
    }

    const unsigned maxSpeed =
        static_cast<unsigned>(std::abs(rpmToMicrostepsPer10kSeconds(m_maxSpeed.getValue())));

    if (!m_controller->setMaxSpeed(maxSpeed))
    {
        LOG(error) << getId() << ": Failed to set max speed (" << m_maxSpeed.getValue() << "/"
                   << maxSpeed << ")";
        return false;
    }

    if (!m_controller->setCurrentLimit(CurrentLimit))
    {
        LOG(error) << getId() << ": Failed to set current limit to " << CurrentLimit << " mA";
        return false;
    }

    // Clear occurred errors
    ByteBuffer buf(2);
    (void)m_controller->getVariableAndClearErrors(0x00, buf);

    // Now exit the safe state and energize!
    if (!m_controller->exitSafeStart() || !m_controller->energize())
    {
        LOG(error) << getId() << ": Failed to prepare motor";
        (void)stop(true);
        return false;
    }

    return true;
}

bool StepperMotor::rotateToPosition(Position position)
{
    assert(m_controller != nullptr);

    if (!prepareForMotion())
    {
        return false;
    }

    const Position currentPosition = getPosition();
    const auto     remainingMotionTime =
        calculateMotionTime(m_maxSpeed.getValue(), std::abs(currentPosition - position));

    if (!m_controller->setTargetPosition(position))
    {
        LOG(error) << getId() << ": Failed to move to target position (" << position << ")";
        (void)stop(true);
        return false;
    }

    return waitAndShutdown(remainingMotionTime);
}

bool StepperMotor::rotate(Direction direction)
{
    assert(m_controller != nullptr);

    if (!prepareForMotion())
    {
        return false;
    }

    const unsigned maxSpeed =
        static_cast<unsigned>(std::abs(rpmToMicrostepsPer10kSeconds(m_maxSpeed.getValue())));
    const int32_t velocity =
        static_cast<int32_t>(maxSpeed) * ((direction == Direction::Forward) ? 1 : -1);

    if (!m_controller->setTargetVelocity(velocity))
    {
        LOG(error) << getId() << ": Failed to set target velocity";
        (void)stop(true);
        return false;
    }

    return true;
}

bool StepperMotor::waitAndShutdown(std::chrono::milliseconds remainingMotionTime,
                                   bool                      stopImmediately)
{
    assert(m_controller != nullptr);

    // FIXME Workaround to ensure that the current velocity is not zero!
    TicController::State state;
    m_controller->getState(state);

    // Wait until motor reached target position!
    bool errorOccurred = false;

    do
    {
        errorOccurred = (!m_controller->getState(state)) ||
                        (state.operationState != TicController::OperationState::Normal);

        if ((state.currentVelocity == 0) || errorOccurred)
        {
            break;
        }

        // Calculate wait time for polling
        const auto startTime = std::chrono::high_resolution_clock::now();
        const auto nextWaitTime =
            (remainingMotionTime > MaxWaitTime) ? MaxWaitTime : remainingMotionTime;
        std::this_thread::sleep_for(nextWaitTime);
        const auto endTime = std::chrono::high_resolution_clock::now();
        const auto elapsedTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        remainingMotionTime = ((remainingMotionTime - elapsedTime) > MinWaitTime)
                                  ? (remainingMotionTime - elapsedTime)
                                  : MinWaitTime;
    } while (!stopImmediately);

    bool shutdownSuccess = m_controller->deEnergize();
    shutdownSuccess      = m_controller->enterSafeStart() && shutdownSuccess;

    if (errorOccurred)
    {
        m_controller->showState(state);
        LOG(error) << getId() << ": Failed to wait for motion stop";
        return false;
    }

    if (!shutdownSuccess)
    {
        LOG(error) << getId() << ": Failed to shutdown motor control";
        return false;
    }

    return true;
}

bool StepperMotor::stop(bool stopImmediately)
{
    bool success = true;
    if (stopImmediately)
    {
        success = m_controller->haltAndHold();  // Stops abruptly!
    }
    else
    {
        success = m_controller->setTargetVelocity(0);
    }

    if (!success)
    {
        LOG(error) << getId() << ": Failed to stop motor";
        stopImmediately = true;  // We do not want to wait!
    }

    return waitAndShutdown(std::chrono::milliseconds(0), stopImmediately);
}

StepperMotor::StepCount StepperMotor::getMicroStepCount() const
{
    return MicroStepsPerStep;
}

StepperMotor::StepCount StepperMotor::getStepsPerRound() const
{
    return MicroStepsPerRound;
}

StepperMotor::Speed StepperMotor::getSpeed() const
{
    TicController::State state;
    if (!m_controller->getState(state))
    {
        LOG(error) << getId() << ": Failed to get current velocity";
        return Speed(0, Unit::Rpm);
    }
    const int valueRpm = microstepsPer10kSecondsToRpm(state.currentVelocity);
    return Speed(valueRpm, Unit::Rpm);
}
