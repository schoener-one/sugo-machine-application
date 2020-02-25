/*
 * CupRotationTray.hpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#ifndef CUPROTATIONTRAY_HPP_
#define CUPROTATIONTRAY_HPP_

#include <string>

#include "CommandExecutionComponent.hpp"
#include "ICommandMessageBroker.hpp"

#include "ICircularMotionController.hpp"
#include "Ident.hpp"
#include "StateMachine.hpp"

namespace moco
{
struct CupRotationTray
{
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

    using Event = moco::Ident<CupRotationTray::EventId>;
    using Fsm   = moco::StateMachine<CupRotationTray::State, CupRotationTray::Event>;
};

std::ostream& operator<<(std::ostream& ostr, CupRotationTray::State const& value);
std::ostream& operator<<(std::ostream& ostr, CupRotationTray::EventId const& value);

class CupRotationTrayImpl final
    : public CupRotationTray,
      public CupRotationTray::Fsm,
      public CommandExecutionComponent<CupRotationTray::State, CupRotationTray::Event>
{
public:
    CupRotationTrayImpl(ICommandMessageBroker&     messageBroker,
                        ICircularMotionController& circularMotionController);

private:
    // Commands
    message::CommandResponse onCommandSwitchOn(const message::Command& command);
    message::CommandResponse onCommandSwitchOff(const message::Command& command);
    message::CommandResponse onCommandReset(const message::Command& command);
    message::CommandResponse onCommandGetState(const message::Command& command);
    message::CommandResponse onCommandGetPosition(const message::Command& command);
    message::CommandResponse onCommandSetPosition(const message::Command& command);
    message::CommandResponse onCommandSwitchOnTest(const message::Command& command);
    message::CommandResponse onCommandTestRotation(const message::Command& command);

    // Transitions
    void switchOn(const CupRotationTray::Event& event, const CupRotationTray::State& state);
    void initialize(const CupRotationTray::Event& event, const CupRotationTray::State& state);
    void switchOff(const CupRotationTray::Event& event, const CupRotationTray::State& state);
    void runSelfTest(const CupRotationTray::Event& event, const CupRotationTray::State& state);
    void handleFailure(const CupRotationTray::Event& event, const CupRotationTray::State& state);
    void rotate(const CupRotationTray::Event& event, const CupRotationTray::State& state);
    void finishSelfTest(const CupRotationTray::Event& event, const CupRotationTray::State& state);
    void testRotation(const CupRotationTray::Event& event, const CupRotationTray::State& state);

private:
    struct TestRotationParameters
    {
        TestRotationParameters() : steps(0), direction(IStepperMotor::Forward) {}
        unsigned                 steps;
        IStepperMotor::Direction direction;
    };
    unsigned                   m_trayPosition = 0;
    ICircularMotionController& m_circularMotionController;
    TestRotationParameters     m_testRotationParameters;
};

}  // namespace moco

#endif /* CUPROTATIONTRAY_HPP_ */
