
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

#include "IFilamentTensionSensor.hpp"

namespace sugo
{
/**
 * @class FilamentTensionSensor
 */
class FilamentTensionSensor : public IFilamentTensionSensor
{
public:    
    // Constructor / Destructor
    explicit FilamentTensionSensor(ICommandMessageBroker& messageBroker) :IFilamentTensionSensor(messageBroker)
    {
    }
    virtual ~FilamentTensionSensor()
    {
    }
   
protected:
    
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;

    // Transition actions
    void handleError(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;

};

} // namespace sugo
