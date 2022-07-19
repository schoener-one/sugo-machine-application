
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

#include "IFilamentCoilMotor.hpp"

namespace sugo
{
/**
 * @class FilamentCoilMotor
 */
class FilamentCoilMotor : public IFilamentCoilMotor
{
public:    
    // Constructor / Destructor
    explicit FilamentCoilMotor(ICommandMessageBroker& messageBroker) :IFilamentCoilMotor(messageBroker)
    {
    }
    virtual ~FilamentCoilMotor()
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
