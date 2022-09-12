
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
    explicit FilamentCoilMotor(ICommandMessageBroker& messageBroker,
                               const ServiceLocator&  serviceLocator)
        : IFilamentCoilMotor(messageBroker), MotorService(serviceLocator)
    {
    }
    virtual ~FilamentCoilMotor()
    {
    }

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandStartMotor(const message::Command& command) override;
    message::CommandResponse onCommandStopMotor(const message::Command& command) override;
    message::CommandResponse onCommandSetMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandSetMotorOffsetSpeed(const message::Command& command) override;

    // Transition actions
    void switchOff(const Event& event, const State& state) override;
    void stopMotor(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void startMotor(const Event& event, const State& state) override;
};

}  // namespace sugo
