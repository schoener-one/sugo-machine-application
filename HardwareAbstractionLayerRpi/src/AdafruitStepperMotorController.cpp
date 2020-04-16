/*
 * AdafruitStepperMotorController.cpp
 *
 *  Created on: 15.12.2019
 *      Author: denis
 */

#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include <Adafruit_MotorHAT.h>
#include <AdafruitStepperMotorController.hpp>

#include <wiringPi.h>

namespace moco
{
/**
 * IStepperMotor implementation class
 */
class AdafruitMotorControl : public IStepperMotorController::IMotorControl
{
public:
    constexpr static unsigned StepsPerRound = 200u;

    explicit AdafruitMotorControl(Adafruit_StepperMotor& motor) : m_stepperMotor(motor) {}

    static ::Direction toDirection(IMotorControl::Direction direction)
    {
        return (direction == Forward ? ::FORWARD : ::BACKWARD);
    }

    // IStepperMotorController::IStepperMotor {{
    void step(IMotorControl::Direction direction) override
    {
        m_stepperMotor.oneStep(toDirection(direction), MICROSTEP);
    }

    unsigned getMicroStepCount() const override
    {
        return static_cast<unsigned>(Adafruit_StepperMotor::MICROSTEPS);
    }

    unsigned getStepsPerRound() const override { return StepsPerRound * getMicroStepCount(); }
    // IStepperMotorController::IStepperMotor }}

private:
    Adafruit_StepperMotor& m_stepperMotor;
};

/**
 *
 */
class AdafruitStepperMotorController::MotorHAT : public Adafruit_MotorHAT
{
public:
    static constexpr int      DefaultStepperMotorControllerI2cAddress   = 0x60;
    static constexpr unsigned DefaultStepperMotorControllerPwmFrequency = 1600;

    MotorHAT(int      i2cAddress   = DefaultStepperMotorControllerI2cAddress,
             unsigned pwmFrequency = DefaultStepperMotorControllerPwmFrequency)
        : Adafruit_MotorHAT(i2cAddress, static_cast<int>(pwmFrequency), -1, -1),
          m_stepperMotors{AdafruitMotorControl(Adafruit_MotorHAT::getStepper(1)),
                          AdafruitMotorControl(Adafruit_MotorHAT::getStepper(2))}
    {
        wiringPiSetup();
    }

    std::vector<AdafruitMotorControl> m_stepperMotors;
};
}  // namespace moco

using namespace moco;

AdafruitStepperMotorController::AdafruitStepperMotorController()
    : m_motorHat(std::make_unique<MotorHAT>())
{
}

AdafruitStepperMotorController::~AdafruitStepperMotorController() {}

void AdafruitStepperMotorController::reset() { m_motorHat->resetAll(); }

IStepperMotorController::IMotorControl& AdafruitStepperMotorController::getMotorControl(
    unsigned number)
{
    assert(number < StepperMotorCount);
    return m_motorHat->m_stepperMotors[number];
}
