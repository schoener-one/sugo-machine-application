
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
#include "IFilamentMergerControl.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class FilamentMergerControl
 */
class FilamentMergerControl : public IFilamentMergerControl,
                              public ControlService<FilamentMergerControl>
{
public:
    // Constructor / Destructor
    explicit FilamentMergerControl(message::ICommandMessageBroker& messageBroker,
                                   const ServiceLocator&           serviceLocator)
        : IFilamentMergerControl(messageBroker), m_serviceLocator(serviceLocator)
    {
    }

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandStartFeeding(const message::Command& command) override;
    message::CommandResponse onCommandStopFeeding(const message::Command& command) override;
    message::CommandResponse onCommandGetTemperatures(const message::Command& command) override;
    message::CommandResponse onCommandStartHeating(const message::Command& command) override;
    message::CommandResponse onNotificationFilamentPreHeaterTargetTemperatureRangeReached(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentPreHeaterErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentMergerHeaterTargetTemperatureRangeReached(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentMergerHeaterErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentFeederMotorStartMotorSucceeded(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentFeederMotorStopMotorSucceeded(
        const message::Command& command) override;
    message::CommandResponse onNotificationFilamentFeederMotorErrorOccurred(
        const message::Command& command) override;
    message::CommandResponse onCommandSetMotorSpeed(const message::Command& command) override;

    // Transition actions
    void switchOff(const Event& event, const State& state) override;
    void stopMerger(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void startMotor(const Event& event, const State& state) override;
    void notifyHeatedUp(const Event& event, const State& state) override;
    void heatingUp(const Event& event, const State& state) override;
    void notifyRunning(const Event& event, const State& state) override;

private:
    void switchOff();

    const ServiceLocator& m_serviceLocator;
    bool                  m_isPreHeaterTemperatureReached    = false;
    bool                  m_isMergerHeaterTemperatureReached = false;

    friend class ControlService<FilamentMergerControl>;
};

}  // namespace sugo
