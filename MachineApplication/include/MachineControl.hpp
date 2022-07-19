///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2022-07-07
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IMachineControl.hpp"

namespace sugo
{
class MachineControl final : public IMachineControl
{
public:
    explicit MachineControl(ICommandMessageBroker& messageBroker) : IMachineControl(messageBroker)
    {
    }

private:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandGetState(const message::Command& command) override;
    message::CommandResponse onCommandIncreaseMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandDecreaseMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandFilamentMergerControlFeedingRunning(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentMergerControlFeedingStopped(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentMergerControlErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentCoilControlCoilRunning(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentCoilControlCoilStopped(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentCoilControlErrorOccurred(
        const message::Command& command) override;

    // Transition actions
    void waitForStarted(const IMachineControl::Event& event,
                        const IMachineControl::State& state) override;
    void handleError(const IMachineControl::Event& event,
                     const IMachineControl::State& state) override;
    void startMachine(const IMachineControl::Event& event,
                      const IMachineControl::State& state) override;
    void runSelfTest(const IMachineControl::Event& event,
                     const IMachineControl::State& state) override;
    void switchOn(const IMachineControl::Event& event,
                  const IMachineControl::State& state) override;
    void switchOff(const IMachineControl::Event& event,
                   const IMachineControl::State& state) override;

    bool m_isFilamentMergerControlRunning = false;
    bool m_isFilamentCoilControlRunning   = false;
};

} /* namespace sugo */
