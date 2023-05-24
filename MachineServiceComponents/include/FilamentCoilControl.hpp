
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
    
    ~FilamentCoilControl() override
    {
    }

protected:
    // Command handlers
    message::CommandResponse onCommandStartCoil(const message::Command& command) override;

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
    bool                  m_controlTension = false;  // Indicates if tension control is on or off

    friend class ControlService<FilamentCoilControl>;
};

}  // namespace sugo
