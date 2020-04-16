/*
 * CupRotaionTray.cpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#include "CupRotationTray.hpp"
#include "CommandParameterParser.hpp"
#include "ICupRotationTray.hpp"

using namespace std;
using namespace moco;
using Transition = CupRotationTray::Fsm::Transition;

std::ostream& moco::operator<<(std::ostream& ostr, CupRotationTray::State const& value)
{
    switch (value)
    {
        case CupRotationTray::State::Off:
            ostr << "Off";
            break;
        case CupRotationTray::State::Idle:
            ostr << "Idle";
            break;
        case CupRotationTray::State::Test:
            ostr << "Test";
            break;
        case CupRotationTray::State::Initialization:
            ostr << "Initialization";
            break;
        case CupRotationTray::State::SelfTest:
            ostr << "SelfTest";
            break;
        case CupRotationTray::State::Rotation:
            ostr << "Rotation";
            break;
        case CupRotationTray::State::Error:
            ostr << "Error";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

std::ostream& moco::operator<<(std::ostream& ostr, CupRotationTray::EventId const& value)
{
    switch (value)
    {
        case CupRotationTray::EventId::SwitchOff:
            ostr << "SwitchOff";
            break;
        case CupRotationTray::EventId::SwitchOn:
            ostr << "SwitchOn";
            break;
        case CupRotationTray::EventId::SwitchOnTest:
            ostr << "SwitchOnTest";
            break;
        case CupRotationTray::EventId::Init:
            ostr << "Reset";
            break;
        case CupRotationTray::EventId::InitFinished:
            ostr << "InitFinished";
            break;
        case CupRotationTray::EventId::InitFailed:
            ostr << "InitFailed";
            break;
        case CupRotationTray::EventId::RunSelfTest:
            ostr << "RunSelfTest";
            break;
        case CupRotationTray::EventId::SelfTestSucceeded:
            ostr << "SelfTestSucceeded";
            break;
        case CupRotationTray::EventId::SelfTestFailed:
            ostr << "SelfTestFailed";
            break;
        case CupRotationTray::EventId::Rotate:
            ostr << "Rotate";
            break;
        case CupRotationTray::EventId::PositionReached:
            ostr << "PositionReached";
            break;
        case CupRotationTray::EventId::RotationFailed:
            ostr << "RotationFailed";
            break;
        case CupRotationTray::EventId::TestRotation:
            ostr << "TestRotation";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

void CupRotationTrayImpl::setConfiguration(IConfiguration& configuration)
{
    configuration.add(Option("cup-rotation-tray.stepper-motor.index", 0u,
                             "CupRotationTray: stepper motor index"));
    configuration.add(Option("cup-rotation-tray.stepper-motor.speed-rpm", 20u,
                             "CupRotationTray: stepper motor speed in unit rpm"));
    configuration.add(Option("cup-rotation-tray.position-switches.gpio-pin-1", 5u,
                             "CupRotationTray: position switch position 1"));
    configuration.add(Option("cup-rotation-tray.position-switches.gpio-pin-2", 6u,
                             "CupRotationTray: position switch position 2"));
    configuration.add(Option("cup-rotation-tray.position-switches.gpio-pin-3", 12u,
                             "CupRotationTray: position switch position 3"));
}

CupRotationTrayImpl::CupRotationTrayImpl(ICommandMessageBroker&     messageBroker,
                                         ICircularMotionController& circularMotionController)
    : Fsm(Off,
          {
              // clang-format off
        Transition(Off,            Initialization, Event(SwitchOn),         Fsm::MakeAction(&CupRotationTrayImpl::switchOn, this)),
		Transition(Off,            Test,           Event(SwitchOnTest),     Fsm::MakeAction(&CupRotationTrayImpl::initialize, this)),
		Transition(Initialization, Initialization, Event(Init),             Fsm::MakeAction(&CupRotationTrayImpl::initialize, this)),
		Transition(Initialization, SelfTest,       Event(InitFinished),     Fsm::MakeAction(&CupRotationTrayImpl::runSelfTest, this)),
		Transition(SelfTest,       Idle,           Event(SelfTestSucceeded), Fsm::MakeAction(&CupRotationTrayImpl::finishSelfTest, this)),
		Transition(Idle,           Off,            Event(SwitchOff),        Fsm::MakeAction(&CupRotationTrayImpl::switchOff, this)),
		Transition(Idle, 	       Initialization, Event(Init),             Fsm::MakeAction(&CupRotationTrayImpl::initialize, this)),
		Transition(Idle, 	       Rotation,       Event(Rotate),           Fsm::MakeAction(&CupRotationTrayImpl::rotate, this)),
		Transition(Rotation, 	   Idle,           Event(PositionReached)),
		Transition(Rotation, 	   Error,          Event(RotationFailed),   Fsm::MakeAction(&CupRotationTrayImpl::handleFailure, this)),
		Transition(SelfTest,       Error,          Event(SelfTestFailed),   Fsm::MakeAction(&CupRotationTrayImpl::handleFailure, this)),
		Transition(Initialization, Error,          Event(InitFailed),       Fsm::MakeAction(&CupRotationTrayImpl::handleFailure, this)),
		Transition(Error,          Off,            Event(SwitchOff),        Fsm::MakeAction(&CupRotationTrayImpl::switchOff, this)),
		Transition(Error,          Initialization, Event(Init),             Fsm::MakeAction(&CupRotationTrayImpl::initialize, this)),
		Transition(Test, 	       Test,           Event(InitFinished)),
        Transition(Test, 	       Test,           Event(TestRotation),     Fsm::MakeAction(&CupRotationTrayImpl::testRotation, this)),
        Transition(Test,           Off,            Event(SwitchOff),        Fsm::MakeAction(&CupRotationTrayImpl::switchOff, this)),
        Transition(Test,           Off,            Event(InitFailed),       Fsm::MakeAction(&CupRotationTrayImpl::switchOff, this)),
              // clang-format on
          }),
      CommandExecutionComponent<CupRotationTray::State, CupRotationTray::Event>(messageBroker),
      m_circularMotionController(circularMotionController)
{
    registerReceiver(
        command::CupRotationTraySwitchOn,
        std::bind(&CupRotationTrayImpl::onCommandSwitchOn, this, std::placeholders::_1));
    registerReceiver(
        command::CupRotationTraySwitchOff,
        std::bind(&CupRotationTrayImpl::onCommandSwitchOff, this, std::placeholders::_1));
    registerReceiver(command::CupRotationTrayReset,
                     std::bind(&CupRotationTrayImpl::onCommandReset, this, std::placeholders::_1));
    registerReceiver(
        command::CupRotationTrayGetState,
        std::bind(&CupRotationTrayImpl::onCommandGetState, this, std::placeholders::_1));
    registerReceiver(
        command::CupRotationTraySetPosition,
        std::bind(&CupRotationTrayImpl::onCommandSetPosition, this, std::placeholders::_1));
    registerReceiver(
        command::CupRotationTrayGetPosition,
        std::bind(&CupRotationTrayImpl::onCommandGetPosition, this, std::placeholders::_1));
    registerReceiver(
        command::CupRotationTraySwitchOnTest,
        std::bind(&CupRotationTrayImpl::onCommandSwitchOnTest, this, std::placeholders::_1));
    registerReceiver(
        command::CupRotationTrayTestRotation,
        std::bind(&CupRotationTrayImpl::onCommandTestRotation, this, std::placeholders::_1));
}

message::CommandResponse CupRotationTrayImpl::onCommandSwitchOn(const message::Command& command)
{
    return handleCommand(command, Event(SwitchOn), *this);
}

message::CommandResponse CupRotationTrayImpl::onCommandSwitchOff(const message::Command& command)
{
    return handleCommand(command, Event(SwitchOff), *this);
}

message::CommandResponse CupRotationTrayImpl::onCommandReset(const message::Command& command)
{
    return handleCommand(command, Event(Init), *this);
}

message::CommandResponse CupRotationTrayImpl::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command, *this);
}

message::CommandResponse CupRotationTrayImpl::onCommandGetPosition(const message::Command& command)
{
    Json json;
    json["position"]          = m_circularMotionController.getPosition();
    const std::string resonse = json.dump();
    return handleRequestCommand(command, resonse);
}

message::CommandResponse CupRotationTrayImpl::onCommandSetPosition(const message::Command& command)
{
    message::CommandResponse response;
    try
    {
        CommandParameterParser parser(command.parameters());
        m_trayPosition = parser.get<unsigned>("position");
        LOG(debug) << "Received rotation request to position " << m_trayPosition;
        response = handleCommand(command, Event(Rotate), *this);
    }
    catch (std::exception& ex)
    {
        response = handleInvalidParameter(command, ex.what());
    }

    return response;
}

message::CommandResponse CupRotationTrayImpl::onCommandSwitchOnTest(const message::Command& command)
{
    return handleCommand(command, Event(SwitchOnTest), *this);
}

message::CommandResponse CupRotationTrayImpl::onCommandTestRotation(const message::Command& command)
{
    message::CommandResponse response;
    try
    {
        CommandParameterParser parser(command.parameters());
        m_testRotationParameters.steps = parser.get<unsigned>("steps");
        std::string direction          = parser.get<std::string>("direction");
        m_testRotationParameters.direction =
            (direction == "backward") ? IStepperMotor::Backward : IStepperMotor::Forward;
        LOG(debug) << "Received test rotation request for " << m_testRotationParameters.steps
                   << " steps and direction " << m_testRotationParameters.direction;
        response = handleCommand(command, Event(TestRotation), *this);
    }
    catch (std::exception& ex)
    {
        response = handleInvalidParameter(command, ex.what());
    }

    return response;
}

void CupRotationTrayImpl::switchOn(const CupRotationTray::Event& event,
                                   const CupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Switching on CupRotationTray...";
    push(Event(Init));
}

void CupRotationTrayImpl::initialize(const CupRotationTray::Event& event,
                                     const CupRotationTray::State& state)
{
    (void)event;
    LOG(info) << "Initializing CupRotationTray...";
    bool success = m_circularMotionController.reset();
    if (success && (state != Test))
    {
        success = m_circularMotionController.rotateTo(0);
    }
    if (success)
    {
        push(Event(InitFinished));
    }
    else
    {
        LOG(error) << "Failed to reset circular motion controller";
        push(Event(InitFailed));
    }
}

void CupRotationTrayImpl::switchOff(const CupRotationTray::Event& event,
                                    const CupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Switching off CupRotationTray...";
}

void CupRotationTrayImpl::runSelfTest(const CupRotationTray::Event& event,
                                      const CupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Running self-tests...";
    // TODO implement self-test!
    push(Event(SelfTestSucceeded));
}

void CupRotationTrayImpl::handleFailure(const CupRotationTray::Event& event,
                                        const CupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(error) << "Handling failure...";
}

void CupRotationTrayImpl::rotate(const CupRotationTray::Event& event,
                                 const CupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Rotating tray to position " << m_trayPosition << "...";
    const bool success = m_circularMotionController.rotateTo(m_trayPosition);
    if (success)
    {
        push(Event(PositionReached));
    }
    else
    {
        LOG(error) << "Failed to rotate tray to position " << m_trayPosition;
        push(Event(RotationFailed));
    }
}

void CupRotationTrayImpl::finishSelfTest(const CupRotationTray::Event& event,
                                         const CupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Finishing self-test...";
    notify(notification::CupRotationTraySelfTestFinished);
}

void CupRotationTrayImpl::testRotation(const CupRotationTray::Event& event,
                                       const CupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Testing rotation with " << m_testRotationParameters.steps
              << " steps and direction " << m_testRotationParameters.direction;
    const bool success = m_circularMotionController.rotate(m_testRotationParameters.steps,
                                                           m_testRotationParameters.direction);
    if (!success)
    {
        LOG(error) << "Test rotation failed";
    }
    notify(notification::CupRotationTrayTestRotationFinished);
}
