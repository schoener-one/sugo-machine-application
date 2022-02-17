///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.09.2019
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef MACHINECONTROLLER_HPP_
#define MACHINECONTROLLER_HPP_

#include <ostream>
#include <string>

#include <Command.pb.h>

#include "ICommandMessageBroker.hpp"
#include "IMachineController.hpp"
#include "Ident.hpp"
#include "StateMachine.hpp"
#include "StatedServiceComponent.hpp"

namespace sugo
{
std::ostream& operator<<(std::ostream& ostr, IMachineController::State const& value);
std::ostream& operator<<(std::ostream& ostr, IMachineController::EventId const& value);

class MachineController final
    : public IMachineController::Fsm,
      public StatedServiceComponent<IMachineController::State, IMachineController::Event>
{
public:
    explicit MachineController(ICommandMessageBroker& messageBroker);

private:
    // Commands
    message::CommandResponse onCommandSwitchOn(const message::Command& command);
    message::CommandResponse onCommandSwitchOff(const message::Command& command);
    message::CommandResponse onCommandReset(const message::Command& command);
    message::CommandResponse onCommandGetState(const message::Command& command);

    // Transitions
    void switchOn(const IMachineController::Event& event, const IMachineController::State& state);
    void switchOff(const IMachineController::Event& event, const IMachineController::State& state);
    void initialize(const IMachineController::Event& event, const IMachineController::State& state);
    void runSelfTest(const IMachineController::Event& event,
                     const IMachineController::State& state);
    void finishSelfTest(const IMachineController::Event& event,
                        const IMachineController::State& state);
    void handleError(const IMachineController::Event& event,
                     const IMachineController::State& state);
};

} /* namespace sugo */

#endif /* MACHINECONTROLLER_HPP_ */
