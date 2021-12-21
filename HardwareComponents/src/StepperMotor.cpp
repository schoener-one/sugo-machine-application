///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.12.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <thread>

#include "Globals.hpp"
#include "IHardwareAbstractionLayer.hpp"
#include "StepperMotor.hpp"

using namespace moco;

StepperMotor::StepperMotor(const IConfiguration& configuration)
    : m_motorIndex(configuration["index"].get<unsigned>()),
      m_motorSpeedRpm(configuration["speed-rpm"].get<unsigned>()),
      m_isStopped(false)
{
}

StepperMotor::~StepperMotor() {}

void StepperMotor::reset()
{
    auto& hal = IHardwareAbstractionLayer::get();
    hal.getStepperMotorController().reset();
    m_isStopped = true;
}

unsigned StepperMotor::runSteps(unsigned steps, Direction direction, RunGuard runGuard)
{
    auto& hal        = IHardwareAbstractionLayer::get();
    auto& controller = hal.getStepperMotorController();
    m_isStopped      = false;
    assert(m_motorIndex < controller.getStepperMotorCount());
    auto&      motor         = controller.getStepperMotorControl(m_motorIndex);
    const auto timePerStepUs = motor.getTimePerStep(m_motorSpeedRpm);
    steps                    = motor.getMicroStepCount() * steps;  // Calculate raw steps
    IStepperMotorController::IStepperMotorControl::Direction motorDirection =
        (direction == Forward) ? IStepperMotorController::IStepperMotorControl::Forward
                               : IStepperMotorController::IStepperMotorControl::Backward;
    unsigned stepsDone = 0;
    for (; (stepsDone < steps) && (!m_isStopped); stepsDone++)
    {
        // Make one step
        const auto t1 = std::chrono::high_resolution_clock::now();
        motor.step(motorDirection);
        if (!m_isStopped)
        {
            m_isStopped = !runGuard();
        }

        // Assure we keep the motor speed!
        auto timeElapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - t1);
        if (timeElapsedUs < timePerStepUs)
        {
            const auto sleepUs = std::chrono::duration_cast<std::chrono::microseconds>(
                timePerStepUs - timeElapsedUs);
            std::this_thread::sleep_for(sleepUs);
            const auto timeUsedUs = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - t1);
        }
    }
    return stepsDone;
}

void StepperMotor::stop() { m_isStopped = true; }

bool StepperMotor::runSelfTest()
{
    return true;  // FIXME not implemented yet!
}

unsigned StepperMotor::getStepsPerRound() const
{
    auto& hal        = IHardwareAbstractionLayer::get();
    auto& controller = hal.getStepperMotorController();
    return controller.getStepperMotorControl(m_motorIndex).getStepsPerRound();
}
