
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

#include "IFilamentMergerHeater.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class FilamentMergerHeater
 */
class FilamentMergerHeater : public IFilamentMergerHeater
{
public:
    // Constructor / Destructor
    explicit FilamentMergerHeater(ICommandMessageBroker& messageBroker,
                                  const ServiceLocator&  serviceLocator)
        : IFilamentMergerHeater(messageBroker), m_serviceLocator(serviceLocator)
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
    void switchOff(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;

    const ServiceLocator& m_serviceLocator;
};

}  // namespace sugo
