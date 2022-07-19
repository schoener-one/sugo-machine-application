
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-19
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IFilamentFeederMotor.hpp"

namespace sugo
{
/**
 * @class FilamentFeederMotor
 */
class FilamentFeederMotor : public IFilamentFeederMotor
{
public:    
    // Constructor / Destructor
    explicit FilamentFeederMotor(ICommandMessageBroker& messageBroker) :IFilamentFeederMotor(messageBroker)
    {
    }
    virtual ~FilamentFeederMotor()
    {
    }
   
protected:
    
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandGetState(const message::Command& command) override;
    message::CommandResponse onCommandStartMotor(const message::Command& command) override;
    message::CommandResponse onCommandStopMotor(const message::Command& command) override;
    message::CommandResponse onCommandSetMotorSpeed(const message::Command& command) override;

    // Transition actions
    void startMotor(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void stopMotor(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

};

} // namespace sugo
