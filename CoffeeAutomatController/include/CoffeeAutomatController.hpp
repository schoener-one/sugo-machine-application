///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.09.2019
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef COFFEEAUTOMATCONTROLLER_HPP_
#define COFFEEAUTOMATCONTROLLER_HPP_

#include <ostream>
#include <string>

#include <Command.pb.h>

#include "ICoffeeAutomatController.hpp"
#include "ICommandMessageBroker.hpp"
#include "Ident.hpp"
#include "StateMachine.hpp"
#include "StatedServiceComponent.hpp"

namespace moco
{
std::ostream& operator<<(std::ostream& ostr, ICoffeeAutomatController::State const& value);
std::ostream& operator<<(std::ostream& ostr, ICoffeeAutomatController::EventId const& value);

class CoffeeAutomatController final : public ICoffeeAutomatController::Fsm,
                                      public StatedServiceComponent<ICoffeeAutomatController::State,
                                                                    ICoffeeAutomatController::Event>
{
public:
    explicit CoffeeAutomatController(ICommandMessageBroker& messageBroker);

private:
    // Commands
    message::CommandResponse onCommandSwitchOn(const message::Command& command);
    message::CommandResponse onCommandSwitchOff(const message::Command& command);
    message::CommandResponse onCommandReset(const message::Command& command);
    message::CommandResponse onCommandGetState(const message::Command& command);

    // Transitions
    void switchOn(const ICoffeeAutomatController::Event& event,
                  const ICoffeeAutomatController::State& state);
    void switchOff(const ICoffeeAutomatController::Event& event,
                   const ICoffeeAutomatController::State& state);
    void initialize(const ICoffeeAutomatController::Event& event,
                    const ICoffeeAutomatController::State& state);
    void runSelfTest(const ICoffeeAutomatController::Event& event,
                     const ICoffeeAutomatController::State& state);
    void finishSelfTest(const ICoffeeAutomatController::Event& event,
                        const ICoffeeAutomatController::State& state);
    void handleError(const ICoffeeAutomatController::Event& event,
                     const ICoffeeAutomatController::State& state);
};

} /* namespace moco */

#endif /* COFFEEAUTOMATCONTROLLER_HPP_ */
