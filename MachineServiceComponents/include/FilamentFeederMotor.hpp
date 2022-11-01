
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

#include "IFilamentFeederMotor.hpp"
#include "MotorService.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class FilamentFeederMotor
 */
class FilamentFeederMotor : public IFilamentFeederMotor, public MotorService
{
public:
    // Constructor / Destructor
    explicit FilamentFeederMotor(message::ICommandMessageBroker& messageBroker,
                                 const ServiceLocator&  serviceLocator)
        : IFilamentFeederMotor(messageBroker),
          MotorService(config::StepperMotorFeederId, serviceLocator)
    {
    }
    virtual ~FilamentFeederMotor() = default;

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandStartMotor(const message::Command& command) override;
    message::CommandResponse onCommandStopMotor(const message::Command& command) override;
    message::CommandResponse onCommandSetMotorSpeed(const message::Command& command) override;

    // Transition actions
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void stopMotor(const Event& event, const State& state) override;
    void startMotor(const Event& event, const State& state) override;
};

}  // namespace sugo
