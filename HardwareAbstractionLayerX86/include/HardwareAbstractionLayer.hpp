/*
 * HardwareAbstractionLayer.hpp
 *
 *  Created on: 03.01.2020
 *      Author: denis
 */

#ifndef HARDWAREABSTRACTIONLAYERSTUB_HPP_
#define HARDWAREABSTRACTIONLAYERSTUB_HPP_

#include "Globals.hpp"
#include "IConfiguration.hpp"
#include "IHardwareAbstractionLayer.hpp"

namespace sugo
{
std::ostream& operator<<(std::ostream& ostr, IGpioController::PinDirection const& pinDir)
{
    ostr << (pinDir == IGpioController::PinDirection::In ? "In" : "Out");
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, IGpioController::PinState const& pinState)
{
    ostr << (pinState == IGpioController::PinState::High ? "High" : "Low");
    return ostr;
}

std::ostream& operator<<(std::ostream&                                                   ostr,
                         IStepperMotorController::IStepperMotorControl::Direction const& dir)
{
    ostr << (dir == IStepperMotorController::IStepperMotorControl::Direction::Forward ? "Forward"
                                                                                      : "Backward");
    return ostr;
}

/**
 *
 */
class HardwareAbstractionLayer : public IHardwareAbstractionLayer
{
public:
    static void setConfiguration(IConfiguration& configuration)
    {
        configuration.add(Option("hardware-abstraction-layer.gpio.logic-active-high", false,
                                 "HAL: Indicates if GPIO logic is active-high or -low"));
    }

    explicit HardwareAbstractionLayer(const IConfiguration& configuration) { (void)configuration; }

    // IHardwareAbstractionLayer {{
    IGpioController&         getGpioController() override { return m_gpioController; }
    IStepperMotorController& getStepperMotorController() override
    {
        return m_stepperMotorController;
    }
    const IStepperMotorController& getStepperMotorController() const override
    {
        return m_stepperMotorController;
    }
    // IHardwareAbstractionLayer }}

private:
    class GpioControllerStub : public IGpioController
    {
    public:
        bool registerPin(unsigned pin, PinDirection pinDirection) override
        {
            LOG(debug) << "Register PIN " << pin << " for direction " << pinDirection;
            return true;
        }
        bool unregisterPin(unsigned pin) override
        {
            LOG(debug) << "Unregister PIN " << pin;
            return true;
        }
        bool getPinState(unsigned pin, PinState& outPinState) const override
        {
            LOG(debug) << "Get PIN " << pin << " state";
            outPinState = PinState::High;
            return true;
        }
        bool setPinState(unsigned pin, PinState pinState) override
        {
            LOG(debug) << "Set PIN " << pin << " state to " << pinState;
            return true;
        }
        bool waitForPinStateChange(unsigned pin, PinState& outPinState,
                                   int timeoutMS) const override
        {
            LOG(debug) << "Wait for PIN " << pin << " change for " << timeoutMS << " ms";
            outPinState = PinState::Low;
            return false;
        }
    };

    class StepperMotorControllerStub : public IStepperMotorController,
                                       public IStepperMotorController::IStepperMotorControl
    {
    public:
        static constexpr unsigned MicroStepCount = 1u;
        static constexpr unsigned StepsPerRound  = 200u;

        // IStepperMotorController {{
        void     reset() override { LOG(debug) << "Reset stepper motor"; }
        unsigned getStepperMotorCount() const override
        {
            LOG(debug) << "Get stepper motor count ->(1)";
            return 1;
        }
        IStepperMotorController::IStepperMotorControl& getStepperMotorControl(
            unsigned number) override
        {
            LOG(debug) << "Get stepper motor " << number;
            assert(number == 0);
            return *this;
        }
        // IStepperMotorController }}

        // IStepperMotorController::IStepperMotorControl {{
        void     step(Direction direction) override { LOG(debug) << "Step motor " << direction; }
        unsigned getMicroStepCount() const override { return MicroStepCount; }
        unsigned getStepsPerRound() const override { return StepsPerRound; }
        // IStepperMotorController::IStepperMotorControl }}
    };
    GpioControllerStub         m_gpioController;
    StepperMotorControllerStub m_stepperMotorController;
};

}  // namespace sugo

#endif /* HARDWAREABSTRACTIONLAYERSTUB_HPP_ */