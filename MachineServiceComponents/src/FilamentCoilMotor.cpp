
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "FilamentCoilMotor.hpp"
#include "IHardwareAbstractionLayer.hpp"

#include <cassert>

using namespace sugo;
using namespace hal;

namespace
{
const std::string StepperMotorName("coiler");
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

message::CommandResponse FilamentCoilMotor::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse FilamentCoilMotor::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

message::CommandResponse FilamentCoilMotor::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command);
}

message::CommandResponse FilamentCoilMotor::onCommandStartMotor(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartMotor));
}

message::CommandResponse FilamentCoilMotor::onCommandStopMotor(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StopMotor));
}

message::CommandResponse FilamentCoilMotor::onCommandSetMotorSpeed(const message::Command& command)
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

void FilamentCoilMotor::startMotor(const IFilamentCoilMotor::Event&,
                                   const IFilamentCoilMotor::State&)
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

void FilamentCoilMotor::switchOn(const IFilamentCoilMotor::Event&,
                                 const IFilamentCoilMotor::State&)
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

void FilamentCoilMotor::handleError(const IFilamentCoilMotor::Event& event,
                                    const IFilamentCoilMotor::State& state)
{
    stopMotor(event, state);
    notify(NotificationErrorOccurred);
}

void FilamentCoilMotor::switchOff(const IFilamentCoilMotor::Event&,
                                  const IFilamentCoilMotor::State&)
{
    // Nothing to do!
}

void FilamentCoilMotor::stopMotor(const IFilamentCoilMotor::Event&,
                                  const IFilamentCoilMotor::State&)
{
    auto& stepperMotor = getStepperMotor(m_serviceLocator);
    if (stepperMotor->stop(false))
    {
        LOG(warning) << "Failed to stop motor - trying to stop immediately";
        stepperMotor->stop(true);  // stop immediately!
    }
}
