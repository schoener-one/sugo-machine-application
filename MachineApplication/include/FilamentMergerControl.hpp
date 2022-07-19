
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

#include "IFilamentMergerControl.hpp"

namespace sugo
{
/**
 * @class FilamentMergerControl
 */
class FilamentMergerControl : public IFilamentMergerControl
{
public:    
    // Constructor / Destructor
    explicit FilamentMergerControl(ICommandMessageBroker& messageBroker) :IFilamentMergerControl(messageBroker)
    {
    }
    virtual ~FilamentMergerControl()
    {
    }
   
protected:
    
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandStartFeeding(const message::Command& command) override;
    message::CommandResponse onCommandStopFeeding(const message::Command& command) override;
    message::CommandResponse onCommandGetState(const message::Command& command) override;
    message::CommandResponse onCommandGetTemperatures(const message::Command& command) override;
    message::CommandResponse onCommandIncreaseMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandDecreaseMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandFilamentPreHeaterTargetTemperatureReached(const message::Command& command) override;
    message::CommandResponse onCommandFilamentPreHeaterHeatingStopped(const message::Command& command) override;
    message::CommandResponse onCommandFilamentPreHeaterErrorOccurred(const message::Command& command) override;
    message::CommandResponse onCommandFilamentMergerHeaterTargetTemperatureReached(const message::Command& command) override;
    message::CommandResponse onCommandFilamentMergerHeaterHeatingStopped(const message::Command& command) override;
    message::CommandResponse onCommandFilamentMergerHeaterErrorOccurred(const message::Command& command) override;
    message::CommandResponse onCommandFilamentFeederMotorStartMotorSucceeded(const message::Command& command) override;
    message::CommandResponse onCommandFilamentFeederMotorStartMotorFailed(const message::Command& command) override;
    message::CommandResponse onCommandFilamentFeederMotorErrorOccurred(const message::Command& command) override;

    // Transition actions
    void startMotor(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void heatingUp(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void stopMotor(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

};

} // namespace sugo
