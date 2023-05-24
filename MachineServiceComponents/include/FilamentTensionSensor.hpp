
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

#include "FilamentTensionControlService.hpp"
#include "IFilamentTensionSensor.hpp"
#include "ServiceLocator.hpp"

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
                                   const ServiceLocator&           serviceLocator);

protected:
    // Command handlers

    // Transition actions
    void handleError(const Event& event, const State& state) override;
    void switchOn(const Event& event, const State& state) override;
    void switchOff(const Event& event, const State& state) override;

    void onFilamentTensionEvent(FilamentTensionEvent event) override;

private:
    const ServiceLocator& m_serviceLocator;
};

}  // namespace sugo
