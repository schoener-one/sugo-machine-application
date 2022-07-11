///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2022-07-07
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IMachineController.hpp"

namespace sugo
{
class MachineController final : private IMachineController
{
public:
    explicit MachineController(ICommandMessageBroker& messageBroker)
        : IMachineController(messageBroker)
    {
    }

private:
    // Commands
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandGetState(const message::Command& command) override;

    // Transition actions
    void startMachine(const IMachineController::Event& event,
                      const IMachineController::State& state) override;
    void switchOn(const IMachineController::Event& event,
                  const IMachineController::State& state) override;
    void waitForStarted(const IMachineController::Event& event,
                        const IMachineController::State& state) override;
    void handleError(const IMachineController::Event& event,
                     const IMachineController::State& state) override;
    void switchOff(const IMachineController::Event& event,
                   const IMachineController::State& state) override;
    void runSelfTest(const IMachineController::Event& event,
                     const IMachineController::State& state) override;

    bool m_isFilamentMergerStarted = false;
    bool m_isFilamentCoilControlStarted = false;
};

} /* namespace sugo */
