/*
 * CoffeeAutomat.h
 *
 *  Created on: 01.09.2019
 *      Author: denis
 */

#ifndef COFFEEAUTOMAT_HPP_
#define COFFEEAUTOMAT_HPP_

#include <ostream>
#include <string>

#include <Command.pb.h>

#include "CommandExecutionComponent.hpp"
#include "ICommandMessageBroker.hpp"
#include "Ident.hpp"
#include "StateMachine.hpp"

namespace moco
{
struct CoffeeAutomat
{
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
    using Fsm   = moco::StateMachine<CoffeeAutomat::State, CoffeeAutomat::Event>;
};

std::ostream& operator<<(std::ostream& ostr, CoffeeAutomat::State const& value);
std::ostream& operator<<(std::ostream& ostr, CoffeeAutomat::EventId const& value);

class CoffeeAutomatImpl final
    : public CoffeeAutomat,
      public CoffeeAutomat::Fsm,
      public CommandExecutionComponent<CoffeeAutomat::State, CoffeeAutomat::Event>
{
public:
    explicit CoffeeAutomatImpl(ICommandMessageBroker& messageBroker);

private:
    // Commands
    message::CommandResponse onCommandSwitchOn(const message::Command& command);
    message::CommandResponse onCommandSwitchOff(const message::Command& command);
    message::CommandResponse onCommandReset(const message::Command& command);
    message::CommandResponse onCommandGetState(const message::Command& command);

    // Transitions
    void switchOn(const CoffeeAutomat::Event& event, const CoffeeAutomat::State& state);
    void switchOff(const CoffeeAutomat::Event& event, const CoffeeAutomat::State& state);
    void initialize(const CoffeeAutomat::Event& event, const CoffeeAutomat::State& state);
    void runSelfTest(const CoffeeAutomat::Event& event, const CoffeeAutomat::State& state);
    void finishSelfTest(const CoffeeAutomat::Event& event, const CoffeeAutomat::State& state);
    void handleError(const CoffeeAutomat::Event& event, const CoffeeAutomat::State& state);
};

} /* namespace moco */

/**
 *
 * @param stream
 * @param event
 * @return
 */
std::ostream& operator<<(std::ostream& stream, const moco::CoffeeAutomat::Event& event);

#endif /* COFFEEAUTOMAT_HPP_ */
