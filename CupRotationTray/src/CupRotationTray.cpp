/*
 * CupRotaionTray.cpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#include "CupRotationTray.hpp"
#include "CommandParameterParser.hpp"

using namespace std;
using namespace sugo;
using namespace sugo::ICupRotationTray;
using Transition = ICupRotationTray::Fsm::Transition;

// cppcheck-suppress unusedFunction
std::ostream& sugo::operator<<(std::ostream& ostr, ICupRotationTray::State const& value)
{
    switch (value)
    {
        case ICupRotationTray::State::Off:
            ostr << "Off";
            break;
        case ICupRotationTray::State::Idle:
            ostr << "Idle";
            break;
        case ICupRotationTray::State::Test:
            ostr << "Test";
            break;
        case ICupRotationTray::State::Initialization:
            ostr << "Initialization";
            break;
        case ICupRotationTray::State::SelfTest:
            ostr << "SelfTest";
            break;
        case ICupRotationTray::State::Rotation:
            ostr << "Rotation";
            break;
        case ICupRotationTray::State::Error:
            ostr << "Error";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

std::ostream& sugo::operator<<(std::ostream& ostr, ICupRotationTray::EventId const& value)
{
    switch (value)
    {
        case ICupRotationTray::EventId::SwitchOff:
            ostr << "SwitchOff";
            break;
        case ICupRotationTray::EventId::SwitchOn:
            ostr << "SwitchOn";
            break;
        case ICupRotationTray::EventId::SwitchOnTest:
            ostr << "SwitchOnTest";
            break;
        case ICupRotationTray::EventId::Init:
            ostr << "Reset";
            break;
        case ICupRotationTray::EventId::InitFinished:
            ostr << "InitFinished";
            break;
        case ICupRotationTray::EventId::InitFailed:
            ostr << "InitFailed";
            break;
        case ICupRotationTray::EventId::RunSelfTest:
            ostr << "RunSelfTest";
            break;
        case ICupRotationTray::EventId::SelfTestSucceeded:
            ostr << "SelfTestSucceeded";
            break;
        case ICupRotationTray::EventId::SelfTestFailed:
            ostr << "SelfTestFailed";
            break;
        case ICupRotationTray::EventId::Rotate:
            ostr << "Rotate";
            break;
        case ICupRotationTray::EventId::PositionReached:
            ostr << "PositionReached";
            break;
        case ICupRotationTray::EventId::RotationFailed:
            ostr << "RotationFailed";
            break;
        case ICupRotationTray::EventId::TestRotation:
            ostr << "TestRotation";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

void CupRotationTray::setConfiguration(IConfiguration& configuration)
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

unsigned CupRotationTray::getGpioPin(unsigned number, const IConfiguration& configuration)
{
    const std::string key("cup-rotation-tray.position-switches.gpio-pin-" + std::to_string(number));
    return configuration.getOption(key).get<unsigned>();
}

const IConfiguration& CupRotationTray::getStepperMotorConfiguration(
    const IConfiguration& configuration, IConfiguration& extractedConfiguration)
{
    return configuration.extract("cup-rotation-tray.stepper-motor.", extractedConfiguration);
}

CupRotationTray::CupRotationTray(ICommandMessageBroker&     messageBroker,
                                 ICircularMotionController& circularMotionController)
    : Fsm(Off,
          {
              // clang-format off
        Transition(Off,            Initialization, Event(SwitchOn),         Fsm::MakeAction(&CupRotationTray::switchOn, this)),
		Transition(Off,            Test,           Event(SwitchOnTest),     Fsm::MakeAction(&CupRotationTray::initialize, this)),
		Transition(Initialization, Initialization, Event(Init),             Fsm::MakeAction(&CupRotationTray::initialize, this)),
		Transition(Initialization, SelfTest,       Event(InitFinished),     Fsm::MakeAction(&CupRotationTray::runSelfTest, this)),
		Transition(SelfTest,       Idle,           Event(SelfTestSucceeded), Fsm::MakeAction(&CupRotationTray::finishSelfTest, this)),
		Transition(Idle,           Off,            Event(SwitchOff),        Fsm::MakeAction(&CupRotationTray::switchOff, this)),
		Transition(Idle, 	       Initialization, Event(Init),             Fsm::MakeAction(&CupRotationTray::initialize, this)),
		Transition(Idle, 	       Rotation,       Event(Rotate),           Fsm::MakeAction(&CupRotationTray::rotate, this)),
		Transition(Rotation, 	   Idle,           Event(PositionReached)),
		Transition(Rotation, 	   Error,          Event(RotationFailed),   Fsm::MakeAction(&CupRotationTray::handleFailure, this)),
		Transition(SelfTest,       Error,          Event(SelfTestFailed),   Fsm::MakeAction(&CupRotationTray::handleFailure, this)),
		Transition(Initialization, Error,          Event(InitFailed),       Fsm::MakeAction(&CupRotationTray::handleFailure, this)),
		Transition(Error,          Off,            Event(SwitchOff),        Fsm::MakeAction(&CupRotationTray::switchOff, this)),
		Transition(Error,          Initialization, Event(Init),             Fsm::MakeAction(&CupRotationTray::initialize, this)),
		Transition(Test, 	       Test,           Event(InitFinished)),
        Transition(Test, 	       Test,           Event(TestRotation),     Fsm::MakeAction(&CupRotationTray::testRotation, this)),
        Transition(Test,           Off,            Event(SwitchOff),        Fsm::MakeAction(&CupRotationTray::switchOff, this)),
        Transition(Test,           Off,            Event(InitFailed),       Fsm::MakeAction(&CupRotationTray::switchOff, this)),
              // clang-format on
          }),
      StatedServiceComponent<ICupRotationTray::State, ICupRotationTray::Event>(messageBroker,
                                                                               *this),
      m_circularMotionController(circularMotionController)
{
    registerCommandHandler(Command::SwitchOn, std::bind(&CupRotationTray::onCommandSwitchOn, this,
                                                        std::placeholders::_1));
    registerCommandHandler(Command::SwitchOff, std::bind(&CupRotationTray::onCommandSwitchOff, this,
                                                         std::placeholders::_1));
    registerCommandHandler(
        Command::Reset, std::bind(&CupRotationTray::onCommandReset, this, std::placeholders::_1));
    registerCommandHandler(Command::GetState, std::bind(&CupRotationTray::onCommandGetState, this,
                                                        std::placeholders::_1));
    registerCommandHandler(Command::SetPosition, std::bind(&CupRotationTray::onCommandSetPosition,
                                                           this, std::placeholders::_1));
    registerCommandHandler(Command::GetPosition, std::bind(&CupRotationTray::onCommandGetPosition,
                                                           this, std::placeholders::_1));
    registerCommandHandler(Command::SwitchOnTest, std::bind(&CupRotationTray::onCommandSwitchOnTest,
                                                            this, std::placeholders::_1));
    registerCommandHandler(Command::TestRotation, std::bind(&CupRotationTray::onCommandTestRotation,
                                                            this, std::placeholders::_1));
}

message::CommandResponse CupRotationTray::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(SwitchOn));
}

message::CommandResponse CupRotationTray::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(SwitchOff));
}

message::CommandResponse CupRotationTray::onCommandReset(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(Init));
}

message::CommandResponse CupRotationTray::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command);
}

message::CommandResponse CupRotationTray::onCommandGetPosition(const message::Command& command)
{
    Json json;
    json["position"]          = m_circularMotionController.getPosition();
    const std::string resonse = json.dump();
    return handleRequestCommand(command, resonse);
}

message::CommandResponse CupRotationTray::onCommandSetPosition(const message::Command& command)
{
    message::CommandResponse response;
    try
    {
        CommandParameterParser parser(command.parameters());
        m_trayPosition = parser.get<unsigned>("position");
        LOG(debug) << "Received rotation request to position " << m_trayPosition;
        response = handleStateChangeCommand(command, Event(Rotate));
    }
    catch (std::exception& ex)
    {
        response = handleInvalidParameter(command, ex.what());
    }

    return response;
}

message::CommandResponse CupRotationTray::onCommandSwitchOnTest(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(SwitchOnTest));
}

message::CommandResponse CupRotationTray::onCommandTestRotation(const message::Command& command)
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
        response = handleStateChangeCommand(command, Event(TestRotation));
    }
    catch (std::exception& ex)
    {
        response = handleInvalidParameter(command, ex.what());
    }

    return response;
}

void CupRotationTray::switchOn(const ICupRotationTray::Event& event,
                               const ICupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Switching on CupRotationTray...";
    push(Event(Init));
}

void CupRotationTray::initialize(const ICupRotationTray::Event& event,
                                 const ICupRotationTray::State& state)
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

void CupRotationTray::switchOff(const ICupRotationTray::Event& event,
                                const ICupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Switching off CupRotationTray...";
}

void CupRotationTray::runSelfTest(const ICupRotationTray::Event& event,
                                  const ICupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Running self-tests...";
    // TODO implement self-test!
    push(Event(SelfTestSucceeded));
}

void CupRotationTray::handleFailure(const ICupRotationTray::Event& event,
                                    const ICupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(error) << "Handling failure...";
}

void CupRotationTray::rotate(const ICupRotationTray::Event& event,
                             const ICupRotationTray::State& state)
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

void CupRotationTray::finishSelfTest(const ICupRotationTray::Event& event,
                                     const ICupRotationTray::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Finishing self-test...";
    notify(Command::SelfTestFinished);
}

void CupRotationTray::testRotation(const ICupRotationTray::Event& event,
                                   const ICupRotationTray::State& state)
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
    notify(Command::TestRotationFinished);
}
