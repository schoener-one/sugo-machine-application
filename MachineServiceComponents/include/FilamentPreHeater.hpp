
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

#include "HeaterService.hpp"
#include "IFilamentPreHeater.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @class FilamentPreHeater
 */
class FilamentPreHeater : public IFilamentPreHeater, public HeaterService
{
public:
    // Constructor / Destructor
    explicit FilamentPreHeater(ICommandMessageBroker& messageBroker,
                               const ServiceLocator&  serviceLocator);
    virtual ~FilamentPreHeater() = default;

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;
    message::CommandResponse onCommandGetTemperature(const message::Command& command) override;

    // Transition actions
    void stopHeating(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void startHeating(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

    // HeaterService events
    void onMaxTemperatureReached() override;
    void onMinTemperatureReached() override;

private:
    bool m_hasNotifiedTargetTemperatureReached = false;
};

}  // namespace sugo
