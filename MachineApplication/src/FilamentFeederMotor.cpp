
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentFeederMotor.hpp"
#include "IHardwareAbstractionLayer.hpp"

#include <cassert>

using namespace sugo;
using namespace hal;

namespace
{
const std::string StepperMotorName("feeder");
const std::string StepperMotorControlName("stepper-motor-control");

inline auto& getStepperMotor(const ServiceLocator& serviceLocator)
{
    auto& stepperMotorControl =
        serviceLocator.get<IHardwareAbstractionLayer>().getStepperMotorControllerMap().at(
            StepperMotorControlName);
    assert(stepperMotorControl);
    auto& stepperMotor = stepperMotorControl->getStepperMotorMap().at(StepperMotorName);
    assert(stepperMotor);
    return stepperMotor;
}
}  // namespace

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse FilamentFeederMotor::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentFeederMotor::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentFeederMotor::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command);
}

message::CommandResponse FilamentFeederMotor::onCommandStartMotor(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotor));
}

message::CommandResponse FilamentFeederMotor::onCommandStopMotor(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StopMotor));
}

message::CommandResponse FilamentFeederMotor::onCommandSetMotorSpeed(
    const message::Command& command)
{
    auto& stepperMotor = getStepperMotor(m_serviceLocator);
    m_currentMotorSpeed += 50;
    stepperMotor->setMaxSpeed(IStepperMotor::Speed(m_currentMotorSpeed, Unit::Rpm));
    message::CommandResponse response;
    response.set_id(command.id());
    response.set_result(message::CommandResponse_Result_SUCCESS);
    return response;
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void FilamentFeederMotor::startMotor(const IFilamentFeederMotor::Event&,
                                     const IFilamentFeederMotor::State&)
{
    auto& stepperMotor = getStepperMotor(m_serviceLocator);
    if (stepperMotor->rotate(IStepperMotor::Direction::Forward))
    {
        push(Event(EventId::StartMotorSucceeded));
        notify(NotificationStartMotorSucceeded);
    }
    else
    {
        push(Event(EventId::StartMotorFailed));
        notify(NotificationStartMotorFailed);
    }
}

void FilamentFeederMotor::switchOn(const IFilamentFeederMotor::Event&,
                                   const IFilamentFeederMotor::State&)
{
    auto& stepperMotor = getStepperMotor(m_serviceLocator);
    if (!stepperMotor->reset())
    {
        push(Event(EventId::SwitchOnFailed));
    }
    else
    {
        m_currentMotorSpeed = stepperMotor->getMaxSpeed().getValue();
        push(Event(EventId::SwitchOnSucceeded));
    }
}

void FilamentFeederMotor::handleError(const IFilamentFeederMotor::Event& event,
                                      const IFilamentFeederMotor::State& state)
{
    stopMotor(event, state);
    notify(NotificationErrorOccurred);
}

void FilamentFeederMotor::switchOff(const IFilamentFeederMotor::Event&,
                                    const IFilamentFeederMotor::State&)
{
    // Nothing to do!
}

void FilamentFeederMotor::stopMotor(const IFilamentFeederMotor::Event&,
                                    const IFilamentFeederMotor::State&)
{
    auto& stepperMotor = getStepperMotor(m_serviceLocator);
    if (stepperMotor->stop(false))
    {
        LOG(warning) << "Failed to stop motor - trying to stop immediately";
        stepperMotor->stop(true);  // stop immediately!
    }
}
