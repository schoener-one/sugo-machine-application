/*
 * RequestHandler.cpp
 *
 *  Created on: 20.09.2019
 *      Author: denis
 */

#include <cassert>
#include <functional>

#include <boost/algorithm/string.hpp>

#include "CoffeeAutomatController.hpp"
#include "Globals.hpp"
#include "ICupRotationTray.hpp"

using namespace std;
using namespace moco;
using namespace moco::ICoffeeAutomatController;
using Transition = ICoffeeAutomatController::Fsm::Transition;

// cppcheck-suppress unusedFunction
std::ostream& moco::operator<<(std::ostream& ostr, ICoffeeAutomatController::State const& value)
{
    switch (value)
    {
        case ICoffeeAutomatController::State::Off:
            ostr << "Off";
            break;
        case ICoffeeAutomatController::State::Idle:
            ostr << "Idle";
            break;
        case ICoffeeAutomatController::State::Initialization:
            ostr << "Initialization";
            break;
        case ICoffeeAutomatController::State::SelfTest:
            ostr << "SelfTest";
            break;
        case ICoffeeAutomatController::State::Error:
            ostr << "Error";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

std::ostream& moco::operator<<(std::ostream& ostr, ICoffeeAutomatController::EventId const& value)
{
    switch (value)
    {
        case ICoffeeAutomatController::EventId::SwitchOff:
            ostr << "SwitchOff";
            break;
        case ICoffeeAutomatController::EventId::SwitchOffFailed:
            ostr << "SwitchOffFailed";
            break;
        case ICoffeeAutomatController::EventId::SwitchOn:
            ostr << "SwitchOn";
            break;
        case ICoffeeAutomatController::EventId::SwitchOnFailed:
            ostr << "SwitchOnFailed";
            break;
        case ICoffeeAutomatController::EventId::Init:
            ostr << "Reset";
            break;
        case ICoffeeAutomatController::EventId::Reinit:
            ostr << "Reinit";
            break;
        case ICoffeeAutomatController::EventId::InitFinished:
            ostr << "InitFinished";
            break;
        case ICoffeeAutomatController::EventId::InitFailed:
            ostr << "InitFailed";
            break;
        case ICoffeeAutomatController::EventId::RunSelfTest:
            ostr << "RunSelfTest";
            break;
        case ICoffeeAutomatController::EventId::SelfTestSucceeded:
            ostr << "SelfTestSucceeded";
            break;
        case ICoffeeAutomatController::EventId::SelfTestFailed:
            ostr << "SelfTestFailed";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

// FIXME Make inidividual error handling not only ErrorOccurred!
CoffeeAutomatController::CoffeeAutomatController(ICommandMessageBroker& messageBroker)
    : Fsm(Off,
          {
              // clang-format off
		Transition(Off,            Initialization, Event(SwitchOn),         Fsm::MakeAction(&CoffeeAutomatController::switchOn, this)),
		Transition(Initialization, Initialization, Event(Init),             Fsm::MakeAction(&CoffeeAutomatController::initialize, this)),
		Transition(Initialization, SelfTest,       Event(InitFinished),     Fsm::MakeAction(&CoffeeAutomatController::runSelfTest, this)),
		Transition(SelfTest,       Idle,           Event(SelfTestSucceeded),Fsm::MakeAction(&CoffeeAutomatController::finishSelfTest, this)),
		Transition(Idle,           Off,            Event(SwitchOff),        Fsm::MakeAction(&CoffeeAutomatController::switchOff, this)),
		Transition(Idle, 	       Initialization, Event(Reinit),           Fsm::MakeAction(&CoffeeAutomatController::initialize, this)),
		Transition(Off,            Error,          Event(SwitchOnFailed),   Fsm::MakeAction(&CoffeeAutomatController::handleError, this)),
		Transition(Initialization, Error,          Event(InitFailed),       Fsm::MakeAction(&CoffeeAutomatController::handleError, this)),
		Transition(SelfTest,       Error,          Event(SelfTestFailed),   Fsm::MakeAction(&CoffeeAutomatController::handleError, this)),
		Transition(Idle,           Error,          Event(SwitchOffFailed),  Fsm::MakeAction(&CoffeeAutomatController::handleError, this)),
		Transition(Error,          Off,            Event(SwitchOff),        Fsm::MakeAction(&CoffeeAutomatController::switchOff, this)),
		Transition(Error,          Initialization, Event(Init),             Fsm::MakeAction(&CoffeeAutomatController::initialize, this)),
              // clang-format on
          }),
      StatedServiceComponent<ICoffeeAutomatController::State, ICoffeeAutomatController::Event>(
          messageBroker, *this)
{
    // Commands
    registerCommandHandler(Command::SwitchOn, std::bind(&CoffeeAutomatController::onCommandSwitchOn,
                                                        this, std::placeholders::_1));
    registerCommandHandler(
        Command::SwitchOff,
        std::bind(&CoffeeAutomatController::onCommandSwitchOff, this, std::placeholders::_1));
    registerCommandHandler(Command::Reset, std::bind(&CoffeeAutomatController::onCommandReset, this,
                                                     std::placeholders::_1));
    registerCommandHandler(Command::GetState, std::bind(&CoffeeAutomatController::onCommandGetState,
                                                        this, std::placeholders::_1));
}

// TODO Generalize command handler methods!
message::CommandResponse CoffeeAutomatController::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(SwitchOn));
}

message::CommandResponse CoffeeAutomatController::onCommandSwitchOff(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(SwitchOff));
}

message::CommandResponse CoffeeAutomatController::onCommandReset(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(Reinit));
}

message::CommandResponse CoffeeAutomatController::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command);
}

void CoffeeAutomatController::switchOn(const ICoffeeAutomatController::Event& event,
                                       const ICoffeeAutomatController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Switching on coffee automat...";
    const message::CommandResponse response = send(Command::SwitchOn);
    if (response.result() == message::CommandResponse_Result_SUCCESS)
    {
        push(Event(Init));
    }
    else
    {
        LOG(error) << "Failed to call '" << Command::SwitchOn << "' (" << response.response()
                   << ")";
        push(Event(SwitchOnFailed));
    }
}

void CoffeeAutomatController::switchOff(const ICoffeeAutomatController::Event&,
                                        const ICoffeeAutomatController::State&)
{
    LOG(info) << "Switching off CoffeeAutomatController...";
    const message::CommandResponse response = send(Command::SwitchOff);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        LOG(error) << "Failed to call '" << Command::SwitchOff << "' (" << response.response()
                   << ")";
        push(Event(SwitchOffFailed));
    }
}

void CoffeeAutomatController::initialize(const ICoffeeAutomatController::Event& event,
                                         const ICoffeeAutomatController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << (event == Reinit ? "Reinitializing" : "Initializing")
              << " CoffeeAutomatController...";
    if (event == Reinit)
    {
        const message::CommandResponse response = send(Command::Reset);
        if (response.result() == message::CommandResponse_Result_SUCCESS)
        {
            push(Event(InitFinished));
        }
        else
        {
            LOG(error) << "Failed to call '" << Command::Reset << "' (" << response.response()
                       << ")";
            push(Event(InitFailed));
        }
    }
    else
    {
        push(Event(InitFinished));
    }
}

void CoffeeAutomatController::runSelfTest(const ICoffeeAutomatController::Event& event,
                                          const ICoffeeAutomatController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Running self-test...";
    push(Event(SelfTestSucceeded));
}

void CoffeeAutomatController::finishSelfTest(const ICoffeeAutomatController::Event& event,
                                             const ICoffeeAutomatController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Finishing self-test...";
}

void CoffeeAutomatController::handleError(const ICoffeeAutomatController::Event& event,
                                          const ICoffeeAutomatController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Error occurred...";
}
