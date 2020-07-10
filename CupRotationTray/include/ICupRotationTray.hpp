/*
 * I.hpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#ifndef ICUPROTATIONTRAY_HPP_
#define ICUPROTATIONTRAY_HPP_

#include <string>

#include "CommandId.hpp"
#include "Ident.hpp"
#include "StateMachine.hpp"

namespace moco
{
namespace ICupRotationTray
{
namespace Command
{
static const std::string ReceiverId = "CupRotationTray";
static const CommandId   SwitchOn(ReceiverId, "SwitchOn");
static const CommandId   SwitchOff(ReceiverId, "SwitchOff");
static const CommandId   Reset(ReceiverId, "Reset");
static const CommandId   GetState(ReceiverId, "GetState");
static const CommandId   GetPosition(ReceiverId, "GetPosition");
static const CommandId   SetPosition(ReceiverId, "SetPosition");
static const CommandId   SwitchOnTest(ReceiverId, "SwitchOnTest");
static const CommandId   TestRotation(ReceiverId, "TestRotation");
static const CommandId   SelfTestFinished(ReceiverId, "SelfTestFinished");
static const CommandId   TestRotationFinished(ReceiverId, "TestRotationFinished");
}  // namespace Command

enum State
{
    Off = 0,
    Idle,
    Test,
    Initialization,
    SelfTest,
    Rotation,
    Error
};

enum EventId
{
    SwitchOff = 0,
    SwitchOn,
    SwitchOnTest,
    Init,
    InitFinished,
    InitFailed,
    RunSelfTest,
    SelfTestSucceeded,
    SelfTestFailed,
    Rotate,
    PositionReached,
    RotationFailed,
    TestRotation
};

using Event = moco::Ident<ICupRotationTray::EventId>;
using Fsm   = moco::StateMachine<ICupRotationTray::State, ICupRotationTray::Event>;
}  // namespace ICupRotationTray
}  // namespace moco

#endif /* ICUPROTATIONTRAY_HPP_ */
