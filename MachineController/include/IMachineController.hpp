///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   17.11.2019
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef IMACHINECONTROLLER_HPP_
#define IMACHINECONTROLLER_HPP_

#include <string>

#include "CommandId.hpp"
#include "Ident.hpp"
#include "StateMachine.hpp"

namespace moco
{
namespace IMachineController
{
namespace Command
{
const std::string ReceiverId = "MachineController";
const CommandId   SwitchOn(ReceiverId, "SwitchOn");
const CommandId   SwitchOff(ReceiverId, "SwitchOff");
const CommandId   Reset(ReceiverId, "Reset");
const CommandId   GetState(ReceiverId, "GetState");
const CommandId   SelfTestFinished(ReceiverId, "SelfTestFinished");
}  // namespace Command

enum State
{
    Off = 0,
    Idle,
    Initialization,
    SelfTest,
    Error
};

enum EventId
{
    SwitchOff = 0,
    SwitchOffFailed,
    SwitchOn,
    SwitchOnFailed,
    Init,
    InitFinished,
    InitFailed,
    Reinit,
    RunSelfTest,
    SelfTestSucceeded,
    SelfTestFailed
};

using Event = moco::Ident<EventId>;
using Fsm   = moco::StateMachine<IMachineController::State, IMachineController::Event>;
}  // namespace IMachineController
}  // namespace moco

#endif /* IMACHINECONTROLLER_HPP_ */
