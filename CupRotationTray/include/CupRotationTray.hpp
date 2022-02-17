/*
 * CupRotationTray.hpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#ifndef CUPROTATIONTRAY_HPP_
#define CUPROTATIONTRAY_HPP_

#include <string>

#include <Command.pb.h>

#include "ICircularMotionController.hpp"
#include "ICommandMessageBroker.hpp"
#include "IConfiguration.hpp"
#include "ICupRotationTray.hpp"
#include "StatedServiceComponent.hpp"

namespace sugo
{
std::ostream& operator<<(std::ostream& ostr, ICupRotationTray::State const& value);
std::ostream& operator<<(std::ostream& ostr, ICupRotationTray::EventId const& value);

class CupRotationTray final
    : public ICupRotationTray::Fsm,
      public StatedServiceComponent<ICupRotationTray::State, ICupRotationTray::Event>
{
public:
    static void                  setConfiguration(IConfiguration& configuration);
    static unsigned              getGpioPin(unsigned number, const IConfiguration& configuration);
    static const IConfiguration& getStepperMotorConfiguration(
        const IConfiguration& configuration, IConfiguration& extractedConfiguration);

    CupRotationTray(ICommandMessageBroker&     messageBroker,
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
    void switchOn(const ICupRotationTray::Event& event, const ICupRotationTray::State& state);
    void initialize(const ICupRotationTray::Event& event, const ICupRotationTray::State& state);
    void switchOff(const ICupRotationTray::Event& event, const ICupRotationTray::State& state);
    void runSelfTest(const ICupRotationTray::Event& event, const ICupRotationTray::State& state);
    void handleFailure(const ICupRotationTray::Event& event, const ICupRotationTray::State& state);
    void rotate(const ICupRotationTray::Event& event, const ICupRotationTray::State& state);
    void finishSelfTest(const ICupRotationTray::Event& event, const ICupRotationTray::State& state);
    void testRotation(const ICupRotationTray::Event& event, const ICupRotationTray::State& state);

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

}  // namespace sugo

#endif /* CUPROTATIONTRAY_HPP_ */
