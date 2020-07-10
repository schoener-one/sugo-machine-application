///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   17.11.2019
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef ICOFFEEAUTOMATCONTROLLER_HPP_
#define ICOFFEEAUTOMATCONTROLLER_HPP_

#include <string>

#include "CommandId.hpp"
#include "Ident.hpp"
#include "StateMachine.hpp"

namespace moco
{
namespace ICoffeeAutomatController
{
namespace Command
{
const std::string ReceiverId = "CoffeeAutomatController";
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
using Fsm   = moco::StateMachine<ICoffeeAutomatController::State, ICoffeeAutomatController::Event>;
}  // namespace ICoffeeAutomatController
}  // namespace moco

#endif /* ICOFFEEAUTOMATCONTROLLER_HPP_ */
