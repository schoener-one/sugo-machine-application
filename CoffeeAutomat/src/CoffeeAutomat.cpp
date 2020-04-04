/*
 * RequestHandler.cpp
 *
 *  Created on: 20.09.2019
 *      Author: denis
 */

#include "CoffeeAutomat.hpp"
#include <cassert>
#include <functional>
#include "Globals.hpp"
#include "ICoffeeAutomat.hpp"

#include <boost/algorithm/string.hpp>

#include "ICupRotationTray.hpp"

using namespace std;
using namespace moco;
using Transition = CoffeeAutomat::Fsm::Transition;

// cppcheck-suppress unusedFunction
std::ostream& moco::operator<<(std::ostream& ostr, CoffeeAutomat::State const& value)
{
    switch (value)
    {
        case CoffeeAutomat::State::Off:
            ostr << "Off";
            break;
        case CoffeeAutomat::State::Idle:
            ostr << "Idle";
            break;
        case CoffeeAutomat::State::Initialization:
            ostr << "Initialization";
            break;
        case CoffeeAutomat::State::SelfTest:
            ostr << "SelfTest";
            break;
        case CoffeeAutomat::State::Error:
            ostr << "Error";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

std::ostream& moco::operator<<(std::ostream& ostr, CoffeeAutomat::EventId const& value)
{
    switch (value)
    {
        case CoffeeAutomat::EventId::SwitchOff:
            ostr << "SwitchOff";
            break;
        case CoffeeAutomat::EventId::SwitchOffFailed:
            ostr << "SwitchOffFailed";
            break;
        case CoffeeAutomat::EventId::SwitchOn:
            ostr << "SwitchOn";
            break;
        case CoffeeAutomat::EventId::SwitchOnFailed:
            ostr << "SwitchOnFailed";
            break;
        case CoffeeAutomat::EventId::Init:
            ostr << "Reset";
            break;
        case CoffeeAutomat::EventId::Reinit:
            ostr << "Reinit";
            break;
        case CoffeeAutomat::EventId::InitFinished:
            ostr << "InitFinished";
            break;
        case CoffeeAutomat::EventId::InitFailed:
            ostr << "InitFailed";
            break;
        case CoffeeAutomat::EventId::RunSelfTest:
            ostr << "RunSelfTest";
            break;
        case CoffeeAutomat::EventId::SelfTestSucceeded:
            ostr << "SelfTestSucceeded";
            break;
        case CoffeeAutomat::EventId::SelfTestFailed:
            ostr << "SelfTestFailed";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

// FIXME Make inidividual error handling not only ErrorOccurred!
CoffeeAutomatImpl::CoffeeAutomatImpl(ICommandMessageBroker& messageBroker)
    : Fsm(Off,
          {
              // clang-format off
		Transition(Off,            Initialization, Event(SwitchOn),         Fsm::MakeAction(&CoffeeAutomatImpl::switchOn, this)),
		Transition(Initialization, Initialization, Event(Init),             Fsm::MakeAction(&CoffeeAutomatImpl::initialize, this)),
		Transition(Initialization, SelfTest,       Event(InitFinished),     Fsm::MakeAction(&CoffeeAutomatImpl::runSelfTest, this)),
		Transition(SelfTest,       Idle,           Event(SelfTestSucceeded),Fsm::MakeAction(&CoffeeAutomatImpl::finishSelfTest, this)),
		Transition(Idle,           Off,            Event(SwitchOff),        Fsm::MakeAction(&CoffeeAutomatImpl::switchOff, this)),
		Transition(Idle, 	       Initialization, Event(Reinit),           Fsm::MakeAction(&CoffeeAutomatImpl::initialize, this)),
		Transition(Off,            Error,          Event(SwitchOnFailed),   Fsm::MakeAction(&CoffeeAutomatImpl::handleError, this)),
		Transition(Initialization, Error,          Event(InitFailed),       Fsm::MakeAction(&CoffeeAutomatImpl::handleError, this)),
		Transition(SelfTest,       Error,          Event(SelfTestFailed),   Fsm::MakeAction(&CoffeeAutomatImpl::handleError, this)),
		Transition(Idle,           Error,          Event(SwitchOffFailed),  Fsm::MakeAction(&CoffeeAutomatImpl::handleError, this)),
		Transition(Error,          Off,            Event(SwitchOff),        Fsm::MakeAction(&CoffeeAutomatImpl::switchOff, this)),
		Transition(Error,          Initialization, Event(Init),             Fsm::MakeAction(&CoffeeAutomatImpl::initialize, this)),
              // clang-format on
          }),
      CommandExecutionComponent<CoffeeAutomat::State, CoffeeAutomat::Event>(messageBroker)
{
    // Commands
    registerReceiver(command::CoffeeAutomatSwitchOn,
                     std::bind(&CoffeeAutomatImpl::onCommandSwitchOn, this, std::placeholders::_1));
    registerReceiver(
        command::CoffeeAutomatSwitchOff,
        std::bind(&CoffeeAutomatImpl::onCommandSwitchOff, this, std::placeholders::_1));
    registerReceiver(command::CoffeeAutomatReset,
                     std::bind(&CoffeeAutomatImpl::onCommandReset, this, std::placeholders::_1));
    registerReceiver(command::CoffeeAutomatGetState,
                     std::bind(&CoffeeAutomatImpl::onCommandGetState, this, std::placeholders::_1));
}

// TODO Generalize command handler methods!
message::CommandResponse CoffeeAutomatImpl::onCommandSwitchOn(const message::Command& command)
{
    return handleCommand(command, Event(SwitchOn), *this);
}

message::CommandResponse CoffeeAutomatImpl::onCommandSwitchOff(const message::Command& command)
{
    return handleCommand(command, Event(SwitchOff), *this);
}

message::CommandResponse CoffeeAutomatImpl::onCommandReset(const message::Command& command)
{
    return handleCommand(command, Event(Reinit), *this);
    ;
}

message::CommandResponse CoffeeAutomatImpl::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command, *this);
}

void CoffeeAutomatImpl::switchOn(const CoffeeAutomat::Event& event,
                                 const CoffeeAutomat::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Switching on coffee automat...";
    const message::CommandResponse response = send(command::CupRotationTraySwitchOn);
    if (response.result() == message::CommandResponse_Result_SUCCESS)
    {
        push(Event(Init));
    }
    else
    {
        LOG(error) << "Failed to call '" << command::CupRotationTraySwitchOn << "' ("
                   << response.response() << ")";
        push(Event(SwitchOnFailed));
    }
}

void CoffeeAutomatImpl::switchOff(const CoffeeAutomat::Event&, const CoffeeAutomat::State&)
{
    LOG(info) << "Switching off CoffeeAutomat...";
    const message::CommandResponse response = send(command::CupRotationTraySwitchOff);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        LOG(error) << "Failed to call '" << command::CupRotationTraySwitchOff << "' ("
                   << response.response() << ")";
        push(Event(SwitchOffFailed));
    }
}

void CoffeeAutomatImpl::initialize(const CoffeeAutomat::Event& event,
                                   const CoffeeAutomat::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << (event == Reinit ? "Reinitializing" : "Initializing") << " CoffeeAutomat...";
    if (event == Reinit)
    {
        const message::CommandResponse response = send(command::CupRotationTrayReset);
        if (response.result() == message::CommandResponse_Result_SUCCESS)
        {
            push(Event(InitFinished));
        }
        else
        {
            LOG(error) << "Failed to call '" << command::CupRotationTrayReset << "' ("
                       << response.response() << ")";
            push(Event(InitFailed));
        }
    }
    else
    {
        push(Event(InitFinished));
    }
}

void CoffeeAutomatImpl::runSelfTest(const CoffeeAutomat::Event& event,
                                    const CoffeeAutomat::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Running self-test...";
    push(Event(SelfTestSucceeded));
}

void CoffeeAutomatImpl::finishSelfTest(const CoffeeAutomat::Event& event,
                                       const CoffeeAutomat::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Finishing self-test...";
}

void CoffeeAutomatImpl::handleError(const CoffeeAutomat::Event& event,
                                    const CoffeeAutomat::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Error occurred...";
}
