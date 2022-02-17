///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.09.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include "MachineController.hpp"

#include <cassert>
#include <functional>

#include <boost/algorithm/string.hpp>

#include "Globals.hpp"
#include "ICupRotationTray.hpp"

using namespace std;
using namespace sugo;
using namespace sugo::IMachineController;
using Transition = IMachineController::Fsm::Transition;

std::ostream& sugo::operator<<(std::ostream& ostr, IMachineController::State const& value)
{
    switch (value)
    {
        case IMachineController::State::Off:
            ostr << "Off";
            break;
        case IMachineController::State::Idle:
            ostr << "Idle";
            break;
        case IMachineController::State::Initialization:
            ostr << "Initialization";
            break;
        case IMachineController::State::SelfTest:
            ostr << "SelfTest";
            break;
        case IMachineController::State::Error:
            ostr << "Error";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

std::ostream& sugo::operator<<(std::ostream& ostr, IMachineController::EventId const& value)
{
    switch (value)
    {
        case IMachineController::EventId::SwitchOff:
            ostr << "SwitchOff";
            break;
        case IMachineController::EventId::SwitchOffFailed:
            ostr << "SwitchOffFailed";
            break;
        case IMachineController::EventId::SwitchOn:
            ostr << "SwitchOn";
            break;
        case IMachineController::EventId::SwitchOnFailed:
            ostr << "SwitchOnFailed";
            break;
        case IMachineController::EventId::Init:
            ostr << "Reset";
            break;
        case IMachineController::EventId::Reinit:
            ostr << "Reinit";
            break;
        case IMachineController::EventId::InitFinished:
            ostr << "InitFinished";
            break;
        case IMachineController::EventId::InitFailed:
            ostr << "InitFailed";
            break;
        case IMachineController::EventId::RunSelfTest:
            ostr << "RunSelfTest";
            break;
        case IMachineController::EventId::SelfTestSucceeded:
            ostr << "SelfTestSucceeded";
            break;
        case IMachineController::EventId::SelfTestFailed:
            ostr << "SelfTestFailed";
            break;
        default:
            ASSERT_NOT_REACHABLE;
    }
    return ostr;
}

// FIXME Make inidividual error handling not only ErrorOccurred!
MachineController::MachineController(ICommandMessageBroker& messageBroker)
    : Fsm(Off,
          {
              // clang-format off
		Transition(Off,            Initialization, Event(SwitchOn),         Fsm::MakeAction(&MachineController::switchOn, this)),
		Transition(Initialization, Initialization, Event(Init),             Fsm::MakeAction(&MachineController::initialize, this)),
		Transition(Initialization, SelfTest,       Event(InitFinished),     Fsm::MakeAction(&MachineController::runSelfTest, this)),
		Transition(SelfTest,       Idle,           Event(SelfTestSucceeded),Fsm::MakeAction(&MachineController::finishSelfTest, this)),
		Transition(Idle,           Off,            Event(SwitchOff),        Fsm::MakeAction(&MachineController::switchOff, this)),
		Transition(Idle, 	       Initialization, Event(Reinit),           Fsm::MakeAction(&MachineController::initialize, this)),
		Transition(Off,            Error,          Event(SwitchOnFailed),   Fsm::MakeAction(&MachineController::handleError, this)),
		Transition(Initialization, Error,          Event(InitFailed),       Fsm::MakeAction(&MachineController::handleError, this)),
		Transition(SelfTest,       Error,          Event(SelfTestFailed),   Fsm::MakeAction(&MachineController::handleError, this)),
		Transition(Idle,           Error,          Event(SwitchOffFailed),  Fsm::MakeAction(&MachineController::handleError, this)),
		Transition(Error,          Off,            Event(SwitchOff),        Fsm::MakeAction(&MachineController::switchOff, this)),
		Transition(Error,          Initialization, Event(Init),             Fsm::MakeAction(&MachineController::initialize, this)),
              // clang-format on
          }),
      StatedServiceComponent<IMachineController::State, IMachineController::Event>(messageBroker,
                                                                                   *this)
{
    // Commands
    registerCommandHandler(Command::SwitchOn, std::bind(&MachineController::onCommandSwitchOn, this,
                                                        std::placeholders::_1));
    registerCommandHandler(Command::SwitchOff, std::bind(&MachineController::onCommandSwitchOff,
                                                         this, std::placeholders::_1));
    registerCommandHandler(
        Command::Reset, std::bind(&MachineController::onCommandReset, this, std::placeholders::_1));
    registerCommandHandler(Command::GetState, std::bind(&MachineController::onCommandGetState, this,
                                                        std::placeholders::_1));
}

// TODO Generalize command handler methods!
message::CommandResponse MachineController::onCommandSwitchOn(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(SwitchOn));
}

message::CommandResponse MachineController::onCommandSwitchOff(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(SwitchOff));
}

message::CommandResponse MachineController::onCommandReset(const message::Command& command)
{
    return handleStateChangeCommand(command, Event(Reinit));
}

message::CommandResponse MachineController::onCommandGetState(const message::Command& command)
{
    return handleCommandGetState(command);
}

void MachineController::switchOn(const IMachineController::Event& event,
                                 const IMachineController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Switching on machine controller...";
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

void MachineController::switchOff(const IMachineController::Event&,
                                  const IMachineController::State&)
{
    LOG(info) << "Switching off MachineController...";
    const message::CommandResponse response = send(Command::SwitchOff);
    if (response.result() != message::CommandResponse_Result_SUCCESS)
    {
        LOG(error) << "Failed to call '" << Command::SwitchOff << "' (" << response.response()
                   << ")";
        push(Event(SwitchOffFailed));
    }
}

void MachineController::initialize(const IMachineController::Event& event,
                                   const IMachineController::State& state)
{
    (void)event;
    (void)state;
    LOG(debug) << (event == Reinit ? "Reinitializing" : "Initializing");

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

void MachineController::runSelfTest(const IMachineController::Event& event,
                                    const IMachineController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Running self-test...";
    push(Event(SelfTestSucceeded));
}

void MachineController::finishSelfTest(const IMachineController::Event& event,
                                       const IMachineController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Finishing self-test...";
}

void MachineController::handleError(const IMachineController::Event& event,
                                    const IMachineController::State& state)
{
    (void)event;
    (void)state;
    LOG(info) << "Error occurred...";
}
