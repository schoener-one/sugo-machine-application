
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

#include "IFilamentCoilControl.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class FilamentCoilControl
 */
class FilamentCoilControl : public IFilamentCoilControl
{
public:
    // Constructor / Destructor
    explicit FilamentCoilControl(ICommandMessageBroker& messageBroker,
                                 const ServiceLocator&  serviceLocator)
        : IFilamentCoilControl(messageBroker), m_serviceLocator(serviceLocator)
    {
    }
    virtual ~FilamentCoilControl()
    {
    }

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandStartCoil(const message::Command& command) override;
    message::CommandResponse onCommandStopCoil(const message::Command& command) override;
    message::CommandResponse onCommandFilamentTensionSensorTensionTooLow(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentTensionSensorTensionTooHigh(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentTensionSensorErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentCoilMotorStartMotorSucceeded(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentCoilMotorStartMotorFailed(
        const message::Command& command) override;
    message::CommandResponse onCommandFilamentCoilMotorErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onCommandSetMotorSpeed(const message::Command& command) override;

    // Transition actions
    void switchOff(const Event& event, const State& state) override;
    void startMotor(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void stopMotor(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void notifyRunning(const Event& event, const State& state) override;

private:
    const ServiceLocator& m_serviceLocator;
    int                   m_motorOffsetSpeed = 0;
};

}  // namespace sugo
