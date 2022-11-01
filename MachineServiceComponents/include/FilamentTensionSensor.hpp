
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

#include "IFilamentTensionSensor.hpp"
#include "ServiceLocator.hpp"
#include "FilamentTensionControlService.hpp"

namespace sugo
{
/**
 * @class FilamentTensionSensor
 */
class FilamentTensionSensor : public IFilamentTensionSensor, public FilamentTensionControlService
{
public:
    // Constructor / Destructor
    explicit FilamentTensionSensor(message::ICommandMessageBroker& messageBroker,
                                   const ServiceLocator&  serviceLocator);
    virtual ~FilamentTensionSensor() = default;

protected:
    // Command handlers
    message::CommandResponse onCommandSwitchOn(const message::Command& command) override;
    message::CommandResponse onCommandSwitchOff(const message::Command& command) override;

    // Transition actions
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

    void onFilamentTensionEvent(FilamentTensionEvent event) override;

private:
    const ServiceLocator& m_serviceLocator;
};

}  // namespace sugo
