
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

#include "IFilamentTensionSensor.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class FilamentTensionSensor
 */
class FilamentTensionSensor : public IFilamentTensionSensor
{
public:
    // Constructor / Destructor
    explicit FilamentTensionSensor(message::ICommandMessageBroker& messageBroker,
                                   const ServiceLocator&  serviceLocator)
        : IFilamentTensionSensor(messageBroker), m_serviceLocator(serviceLocator)
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
    void switchOn(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

    const ServiceLocator& m_serviceLocator;
};

}  // namespace sugo
