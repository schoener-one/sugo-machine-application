
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

#include "IMachineControl.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class MachineControl
 */
class MachineControl : public IMachineControl
{
public:
    /// @brief Default motor speed
    static constexpr unsigned MotorSpeedInc = 10;  // FIXME needs to be configurable
    /// @brief Default motor speed
    static constexpr unsigned DefaultMotorSpeed =
        10 * MotorSpeedInc;  // FIXME needs to be configurable
    /// @brief Max motor speed
    static constexpr unsigned MaxMotorSpeed = 30 * MotorSpeedInc;  // FIXME needs to be configurable

    // Constructor / Destructor
    explicit MachineControl(ICommandMessageBroker& messageBroker,
                            const ServiceLocator&  serviceLocator)
        : IMachineControl(messageBroker), m_serviceLocator(serviceLocator)
    {
    }
    virtual ~MachineControl()
    {
    }

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandGetState(const message::Command& command) override;
    message::CommandResponse onCommandIncreaseMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandDecreaseMotorSpeed(const message::Command& command) override;
    message::CommandResponse onCommandGetMotorSpeed(const message::Command& command) override;
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
    void runSelfTest(const Event& event, const State& state) override;
    void startMachine(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void waitForStarted(const Event& event, const State& state) override;

private:
    const ServiceLocator& m_serviceLocator;
    bool                  m_isFilamentMergerControlRunning = false;
    bool                  m_isFilamentCoilControlRunning   = false;
    unsigned              m_currentMotorSpeed              = DefaultMotorSpeed;
};

}  // namespace sugo
