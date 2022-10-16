
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

#include "Globals.hpp"
#include "IRequestHandler.hpp"
#include "IUserInterfaceControl.hpp"
#include "ServiceLocator.hpp"
#include "Thread.hpp"

namespace sugo
{
/**
 * @class UserInterfaceControl
 */
class UserInterfaceControl : public IUserInterfaceControl, public remote_control::IRequestHandler
{
public:
    // Constructor / Destructor
    explicit UserInterfaceControl(message::ICommandMessageBroker& messageBroker,
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
    message::CommandResponse onNotificationMachineControlStarting(
        const message::Command& command) override;
    message::CommandResponse onNotificationMachineControlRunning(
        const message::Command& command) override;
    message::CommandResponse onNotificationMachineControlSwitchedOff(
        const message::Command& command) override;
    message::CommandResponse onNotificationMachineControlErrorOccurred(
        const message::Command& command) override;

    // Transition actions
    void handleMachineStateChange(const Event& event, const State& state) override;

private:
    void               updateMachineState();
    Json               createStateMessage(const std::string& type);
    static std::string convertToString(UserInterfaceControl::EventId event);

    const ServiceLocator&    m_serviceLocator;
    SendNotificationCallback m_cbSendNotification = nullptr;
    EventId                  m_lastMachineEvent   = EventId::MachineSwitchedOff;
};

}  // namespace sugo
