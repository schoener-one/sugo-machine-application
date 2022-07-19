
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

#include "IFilamentMergerHeater.hpp"

namespace sugo
{
/**
 * @class FilamentMergerHeater
 */
class FilamentMergerHeater : public IFilamentMergerHeater
{
public:    
    // Constructor / Destructor
    explicit FilamentMergerHeater(ICommandMessageBroker& messageBroker) :IFilamentMergerHeater(messageBroker)
    {
    }
    virtual ~FilamentMergerHeater()
    {
    }
   
protected:
    
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandGetTemperature(const message::Command& command) override;

    // Transition actions
    void handleError(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;

};

} // namespace sugo
