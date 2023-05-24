
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IFilamentCoilMotor.hpp"
#include "MotorService.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class FilamentCoilMotor
 */
class FilamentCoilMotor : public IFilamentCoilMotor, public MotorService
{
public:
    // Constructor / Destructor
    explicit FilamentCoilMotor(message::ICommandMessageBroker& messageBroker,
                               const ServiceLocator&           serviceLocator)
        : IFilamentCoilMotor(messageBroker),
          MotorService(hal::id::StepperMotorCoiler, serviceLocator),
          m_serviceLocator(serviceLocator)
    {
    }

protected:
    // Command handlers
    message::CommandResponse onCommandSetMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandIncreaseMotorOffsetSpeed(
        const message::Command& command) override;
    message::CommandResponse onCommandDecreaseMotorOffsetSpeed(
        const message::Command& command) override;

    // Transition actions
    void handleError(const Event& event, const State& state) override;
    void stopMotor(const Event& event, const State& state) override;
    void startMotor(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;

private:
    const ServiceLocator& m_serviceLocator;
};

}  // namespace sugo
