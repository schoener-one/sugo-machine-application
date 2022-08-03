
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-07-29
 */
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceControl.hpp"
#include "IHardwareAbstractionLayer.hpp"

#include <cassert>

using namespace sugo;
using namespace hal;

namespace
{
const std::string GpioControl("gpio-control");
const std::string GpioPinRelaySwitchLightRun("relay-switch-light-run");
const std::string GpioPinRelaySwitchLightPower("relay-switch-light-power");
const std::string GpioPinRelaySwitchLightReady("relay-switch-light-ready");
const std::string GpioPinSignalButtonStart("signal-button-start");
const std::string GpioPinSignalButtonStop("signal-button-stop");

inline auto& getGpioPin(const ServiceLocator& serviceLocator, const std::string& gpioPinName)
{
    auto& gpioController =
        serviceLocator.get<IHardwareAbstractionLayer>().getGpioControllerMap().at(GpioControl);
    assert(gpioController);
    auto& gpioPin = gpioController->getGpioPinMap().at(gpioPinName);
    assert(gpioPin);
    return gpioPin;
}
}  // namespace

///////////////////////////////////////////////////////////////////////////////
// Commands:

message::CommandResponse UserInterfaceControl::onCommandMachineControlStarting(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOn));
}

message::CommandResponse UserInterfaceControl::onCommandMachineControlRunning(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::StartingFinished));
}

message::CommandResponse UserInterfaceControl::onCommandMachineControlSwitchedOff(
    const message::Command& command)
{
    return handleStateChangeCommand(command, Event(EventId::SwitchOff));
}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:

void UserInterfaceControl::switchOff(const IUserInterfaceControl::Event&,
                                     const IUserInterfaceControl::State&)
{
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightRun)->setState(IGpioPin::State::Low);
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightReady)
        ->setState(IGpioPin::State::Low);
}

void UserInterfaceControl::switchOn(const IUserInterfaceControl::Event&,
                                    const IUserInterfaceControl::State&)
{
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightRun)->setState(IGpioPin::State::High);
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightReady)
        ->setState(IGpioPin::State::Low);
    auto& buttonStart = getGpioPin(m_serviceLocator, GpioPinSignalButtonStart);
    m_signalButtonStartObserver.start(
        buttonStart, [&](const IGpioPin::Event&) { notify(NotificationUserSwitchOn); });
    m_signalButtonStopObserver.stop();
}

void UserInterfaceControl::switchRunning(const IUserInterfaceControl::Event&,
                                         const IUserInterfaceControl::State&)
{
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightRun)->setState(IGpioPin::State::High);
    (void)getGpioPin(m_serviceLocator, GpioPinRelaySwitchLightReady)
        ->setState(IGpioPin::State::High);
    auto& buttonStop = getGpioPin(m_serviceLocator, GpioPinSignalButtonStop);
    m_signalButtonStopObserver.start(
        buttonStop, [&](const IGpioPin::Event&) { notify(NotificationUserSwitchOff); });
    m_signalButtonStartObserver.stop();
}
