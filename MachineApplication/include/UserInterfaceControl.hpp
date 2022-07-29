
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

#include "IUserInterfaceControl.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class UserInterfaceControl
 */
class UserInterfaceControl : public IUserInterfaceControl
{
public:
    // Constructor / Destructor
    explicit UserInterfaceControl(ICommandMessageBroker& messageBroker,
                                  const ServiceLocator&  serviceLocator)
        : IUserInterfaceControl(messageBroker), m_serviceLocator(serviceLocator)
    {
    }
    virtual ~UserInterfaceControl()
    {
    }

protected:
    // Command handlers
    message::CommandResponse onCommandMachineControlStarting(
        const message::Command& command) override;
    message::CommandResponse onCommandMachineControlRunning(
        const message::Command& command) override;
    message::CommandResponse onCommandMachineControlSwitchedOff(
        const message::Command& command) override;

    // Transition actions
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

    const ServiceLocator& m_serviceLocator;
};

}  // namespace sugo
