
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
    explicit FilamentPreHeater(message::ICommandMessageBroker& messageBroker,
                               const ServiceLocator&           serviceLocator);

protected:
    // Command handlers
    message::CommandResponse onCommandGetTemperature(const message::Command& command) override;

    // Transition actions
    void stopHeating(const Event& event, const State& state) override;
    void handleError(const Event& event, const State& state) override;
    void startHeating(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

    // HeaterService events
    void onTemperatureLimitEvent(TemperatureLimitEvent event) override;
};

}  // namespace sugo
