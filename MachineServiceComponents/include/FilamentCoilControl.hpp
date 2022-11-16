
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
#include "IFilamentCoilControl.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class FilamentCoilControl
 */
class FilamentCoilControl : public IFilamentCoilControl, public ControlService<FilamentCoilControl>
{
public:
    // Constructor / Destructor
    explicit FilamentCoilControl(message::ICommandMessageBroker& messageBroker,
                                 const ServiceLocator&           serviceLocator)
        : IFilamentCoilControl(messageBroker), m_serviceLocator(serviceLocator)
    {
    }

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandStartCoil(const message::Command& command) override;
    message::CommandResponse onCommandStopCoil(const message::Command& command) override;
    message::CommandResponse onNotificationFilamentTensionSensorTensionTooLow(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentTensionSensorTensionTooHigh(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentTensionSensorErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentCoilMotorStartMotorSucceeded(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentCoilMotorStartMotorFailed(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentCoilMotorErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onCommandSetMotorSpeed(const message::Command& command) override;

    // Transition actions
    void switchOff(const Event& event, const State& state) override;
    void startMotor(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void stopMotor(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void notifyRunning(const Event& event, const State& state) override;
    void controlFilamentTension(const Event& event, const State& state) override;

private:
    void switchOff();

    const ServiceLocator& m_serviceLocator;

    friend class ControlService<FilamentCoilControl>;
};

}  // namespace sugo
