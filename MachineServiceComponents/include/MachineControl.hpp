
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

#include "ControlService.hpp"
#include "IMachineControl.hpp"
#include "MachineConfig.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class MachineControl
 */
class MachineControl : public IMachineControl, public ControlService<MachineControl>
{
public:
    // Constructor / Destructor
    explicit MachineControl(message::ICommandMessageBroker& messageBroker,
                            const ServiceLocator&           serviceLocator);

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandStart(const message::Command& command) override;
    message::CommandResponse onCommandStartHeatless(const message::Command& command) override;
    message::CommandResponse onCommandIncreaseMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandDecreaseMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandStop(const message::Command& command) override;
    message::CommandResponse onCommandGetMotorSpeed(const message::Command& command) override;
    message::CommandResponse onNotificationFilamentMergerControlFeedingRunning(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentMergerControlFeedingStopped(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentMergerControlHeatedUp(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentMergerControlErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentCoilControlCoilRunning(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentCoilControlCoilStopped(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentCoilControlErrorOccurred(
        const message::Command& command) override;

    // Transition actions
    void switchOn(const Event& event, const State& state) override;
    void startHeating(const Event& event, const State& state) override;
    void stopMachine(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;
    void checkStartingState(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void notifyStopped(const Event& event, const State& state) override;
    void startMachine(const Event& event, const State& state) override;
    void checkStoppingState(const Event& event, const State& state) override;

private:
    void switchOff();

    const ServiceLocator& m_serviceLocator;
    bool                  m_isFilamentMergerControlRunning = false;
    bool                  m_isFilamentCoilControlRunning   = false;
    unsigned              m_motorSpeed                     = 0;

    friend class ControlService<MachineControl>;
};

}  // namespace sugo
