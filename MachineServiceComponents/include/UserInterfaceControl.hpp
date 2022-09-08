
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

#include "IRequestHandler.hpp"
#include "IUserInterfaceControl.hpp"
#include "ServiceLocator.hpp"
#include "Thread.hpp"
#include "Globals.hpp"

namespace sugo
{
/**
 * @class UserInterfaceControl
 */
class UserInterfaceControl : public IUserInterfaceControl, public remote_control::IRequestHandler
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

    bool receiveRequest(remote_control::IRequestHandler::ClientId clientId, const Json& request,
                        Json& response) override;
    void registerSendNotification(SendNotificationCallback callback)
    {
        m_cbSendNotification = callback;
        updateMachineState();
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
    void switchOn(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;
    void switchRunning(const Event& event, const State& state) override;

private:
    void updateMachineState();
    Json createStateMessage(const std::string& type);
    
    const ServiceLocator&    m_serviceLocator;
    SendNotificationCallback m_cbSendNotification = nullptr;
};

}  // namespace sugo
