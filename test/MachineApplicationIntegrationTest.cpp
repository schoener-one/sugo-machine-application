///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <algorithm>

#include "Common/IConfigurationMock.hpp"
#include "Common/IOContext.hpp"
#include "Common/Logger.hpp"
#include "Common/ServiceLocator.hpp"
#include "Common/Thread.hpp"
#include "Common/Types.hpp"
#include "HardwareAbstractionLayer/IGpioControlMock.hpp"
#include "HardwareAbstractionLayer/IGpioPinMock.hpp"
#include "HardwareAbstractionLayer/IHardwareAbstractionLayerMock.hpp"
#include "HardwareAbstractionLayer/IStepperMotorControlMock.hpp"
#include "HardwareAbstractionLayer/IStepperMotorMock.hpp"
#include "HardwareAbstractionLayer/ITemperatureSensorControlMock.hpp"
#include "HardwareAbstractionLayer/ITemperatureSensorMock.hpp"
#include "IntegrationTest.hpp"
#include "MachineServiceComponent/ExecutionGroup.hpp"
#include "MachineServiceComponent/UserInterfaceControl.hpp"
#include "MachineServiceComponentTest/MachineConfiguration.hpp"
#include "MessageBroker/MessageBroker.hpp"
#include "ServiceComponent/IServiceComponent.hpp"

using namespace sugo;
using namespace sugo::message_broker;
using namespace sugo::hal;
using namespace sugo::remote_control;
using namespace sugo::service_component;
using namespace sugo::machine_service_component;

using ::testing::_;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

namespace
{
MATCHER_P(speedEqual, other, "")
{
    return (arg == other);
}
}  // namespace

namespace sugo
{
class MachineApplicationIntegrationTest : public IntegrationTest
{
protected:
    inline static const message_broker::Address& MyAddress{"MachineApplicationIntegrationTest"};

    MachineApplicationIntegrationTest() : IntegrationTest(MyAddress)
    {
    }

    void SetUp() override
    {
        IntegrationTest::SetUp();
        m_serviceLocator.add<common::IConfiguration>(m_mockConfiguration);
        m_machineConfig.prepareOptions(m_mockConfiguration);
        prepareForStartComponents();

        m_execGroup = std::make_unique<machine_service_component::ExecutionGroup>(
            machine_service_component::MachineControlBundle{m_serviceLocator},
            machine_service_component::FilamentMergerControlBundle{m_serviceLocator},
            machine_service_component::FilamentFeederMotorBundle{m_serviceLocator},
            machine_service_component::FilamentMergerHeaterBundle{m_serviceLocator},
            machine_service_component::FilamentPreHeaterBundle{m_serviceLocator},
            machine_service_component::FilamentCoilControlBundle{m_serviceLocator},
            machine_service_component::FilamentCoilMotorBundle{m_serviceLocator},
            machine_service_component::FilamentTensionSensorBundle{m_serviceLocator},
            machine_service_component::UserInterfaceControlBundle{m_serviceLocator});

        auto& serviceComponent =
            m_execGroup->get<machine_service_component::UserInterfaceControlBundle>();
        serviceComponent.getComponent().registerSendNotification(
            m_mockNotificationHandler.AsStdFunction());

        EXPECT_TRUE(m_execGroup->start());
    }

    void TearDown() override
    {
        IntegrationTest::TearDown();
        m_execGroup->stop();
        m_execGroup.reset();  // Call destructor!
    }

    static void SetUpTestCase()
    {
        common::Logger::init(common::Logger::Severity::debug, MyAddress);
    }

    void prepareHardwareAbstractionLayer();
    void prepareForStartComponents();
    void prepareSwitchOn(bool);
    void heatUp();
    void coolDown();
    void switchOnMachine();
    void switchOffMachine();

    IHardwareAbstractionLayer::StepperMotorControllerMap      m_stepperMotorControllerMap;
    IHardwareAbstractionLayer::TemperatureSensorControllerMap m_temperatureSensorControllerMap;
    IHardwareAbstractionLayer::GpioControllerMap              m_gpioControllerMap;

    IStepperMotorControl::StepperMotorMap               m_stepperMotorMap;
    std::shared_ptr<NiceMock<IStepperMotorControlMock>> m_mockStepperMotorControl;
    std::shared_ptr<NiceMock<IStepperMotorMock>>        m_mockStepperMotorCoiler;
    std::shared_ptr<NiceMock<IStepperMotorMock>>        m_mockStepperMotorFeeder;

    ITemperatureSensorControl::TemperatureSensorMap          m_temperatureSensorMap;
    std::shared_ptr<NiceMock<ITemperatureSensorControlMock>> m_mockTemperatureSensorControl;
    std::shared_ptr<NiceMock<ITemperatureSensorMock>>        m_mockTemperatureSensorMerger;
    std::shared_ptr<NiceMock<ITemperatureSensorMock>>        m_mockTemperatureSensorFeeder;

    IGpioControl::GpioPinMap                    m_gpioPinMap;
    std::shared_ptr<NiceMock<IGpioControlMock>> m_mockGpioControl;
    std::shared_ptr<NiceMock<IGpioPinMock>>     m_mockGpioPinRelaySwitchHeaterFeeder;
    std::shared_ptr<NiceMock<IGpioPinMock>>     m_mockGpioPinRelaySwitchHeaterMerger;
    std::shared_ptr<NiceMock<IGpioPinMock>>     m_mockGpioPinRelaySwitchLightRun;
    std::shared_ptr<NiceMock<IGpioPinMock>>     m_mockGpioPinRelaySwitchLightPower;
    std::shared_ptr<NiceMock<IGpioPinMock>>     m_mockGpioPinRelaySwitchLightReady;
    std::shared_ptr<NiceMock<IGpioPinMock>>     m_mockGpioPinSignalFilamentTensionLow;
    std::shared_ptr<NiceMock<IGpioPinMock>>     m_mockGpioPinSignalFilamentTensionHigh;
    std::shared_ptr<NiceMock<IGpioPinMock>>     m_mockGpioPinSignalFilamentTensionOverload;

    NiceMock<MockFunction<void(const common::Json&)>> m_mockNotificationHandler;

    NiceMock<common::IConfigurationMock>                       m_mockConfiguration;
    NiceMock<IHardwareAbstractionLayerMock>                    m_mockHardwareAbstractionLayer;
    test::MachineConfiguration                                 m_machineConfig;
    common::ServiceLocator                                     m_serviceLocator;
    std::unique_ptr<machine_service_component::ExecutionGroup> m_execGroup;

    unsigned m_nextMotorSpeed = 0;
};  // namespace IntegrationTest

void MachineApplicationIntegrationTest::prepareHardwareAbstractionLayer()
{
    m_stepperMotorControllerMap.emplace(
        hal::id::StepperMotorControl,
        m_mockStepperMotorControl = std::make_shared<NiceMock<IStepperMotorControlMock>>());
    m_stepperMotorMap.emplace(
        hal::id::StepperMotorCoiler,
        m_mockStepperMotorCoiler = std::make_shared<NiceMock<IStepperMotorMock>>());
    m_stepperMotorMap.emplace(
        hal::id::StepperMotorFeeder,
        m_mockStepperMotorFeeder = std::make_shared<NiceMock<IStepperMotorMock>>());

    m_temperatureSensorControllerMap.emplace(
        hal::id::TemperatureSensorControl,
        m_mockTemperatureSensorControl =
            std::make_shared<NiceMock<ITemperatureSensorControlMock>>());
    m_temperatureSensorMap.emplace(
        hal::id::TemperatureSensorMerger,
        m_mockTemperatureSensorMerger = std::make_shared<NiceMock<ITemperatureSensorMock>>());
    m_temperatureSensorMap.emplace(
        hal::id::TemperatureSensorFeeder,
        m_mockTemperatureSensorFeeder = std::make_shared<NiceMock<ITemperatureSensorMock>>());

    m_gpioControllerMap.emplace(hal::id::GpioControl,
                                m_mockGpioControl = std::make_shared<NiceMock<IGpioControlMock>>());
    m_gpioPinMap.emplace(
        hal::id::GpioPinRelaySwitchHeaterFeeder,
        m_mockGpioPinRelaySwitchHeaterFeeder =
            std::make_shared<NiceMock<IGpioPinMock>>("GpioPinRelaySwitchFeederHeater"));
    m_gpioPinMap.emplace(
        hal::id::GpioPinRelaySwitchHeaterMerger,
        m_mockGpioPinRelaySwitchHeaterMerger =
            std::make_shared<NiceMock<IGpioPinMock>>("GpioPinRelaySwitchMergerHeater"));
    m_gpioPinMap.emplace(
        hal::id::GpioPinRelaySwitchLightRun,
        m_mockGpioPinRelaySwitchLightRun =
            std::make_shared<NiceMock<IGpioPinMock>>("GpioPinRelaySwitchLightRun"));
    m_gpioPinMap.emplace(
        hal::id::GpioPinRelaySwitchLightPower,
        m_mockGpioPinRelaySwitchLightPower =
            std::make_shared<NiceMock<IGpioPinMock>>("GpioPinRelaySwitchLightPower"));
    m_gpioPinMap.emplace(
        hal::id::GpioPinRelaySwitchLightReady,
        m_mockGpioPinRelaySwitchLightReady =
            std::make_shared<NiceMock<IGpioPinMock>>("GpioPinRelaySwitchLightReady"));
    m_gpioPinMap.emplace(
        hal::id::GpioPinSignalFilamentTensionLow,
        m_mockGpioPinSignalFilamentTensionLow =
            std::make_shared<NiceMock<IGpioPinMock>>("GpioPinSignalFilamentTensionLow"));
    m_gpioPinMap.emplace(
        hal::id::GpioPinSignalFilamentTensionHigh,
        m_mockGpioPinSignalFilamentTensionHigh =
            std::make_shared<NiceMock<IGpioPinMock>>("GpioPinSignalFilamentTensionHigh"));
    m_gpioPinMap.emplace(
        hal::id::GpioPinSignalFilamentTensionOverload,
        m_mockGpioPinSignalFilamentTensionOverload =
            std::make_shared<NiceMock<IGpioPinMock>>("GpioPinSignalFilamentTensionOverload"));

    ON_CALL(m_mockHardwareAbstractionLayer, getStepperMotorControllerMap())
        .WillByDefault(ReturnRef(m_stepperMotorControllerMap));
    ON_CALL(*m_mockStepperMotorControl, getStepperMotorMap())
        .WillByDefault(ReturnRef(m_stepperMotorMap));
    ON_CALL(m_mockHardwareAbstractionLayer, getTemperatureSensorControllerMap())
        .WillByDefault(ReturnRef(m_temperatureSensorControllerMap));
    ON_CALL(*m_mockTemperatureSensorControl, getTemperatureSensorMap())
        .WillByDefault(ReturnRef(m_temperatureSensorMap));
    ON_CALL(m_mockHardwareAbstractionLayer, getGpioControllerMap())
        .WillByDefault(ReturnRef(m_gpioControllerMap));
    ON_CALL(*m_mockGpioControl, getGpioPinMap()).WillByDefault(ReturnRef(m_gpioPinMap));
    m_serviceLocator.add<IHardwareAbstractionLayer>(m_mockHardwareAbstractionLayer);
}

void MachineApplicationIntegrationTest::prepareForStartComponents()
{
    prepareHardwareAbstractionLayer();
    ON_CALL(*m_mockStepperMotorCoiler, reset()).WillByDefault(Return(true));
    ON_CALL(*m_mockStepperMotorCoiler, getMaxSpeed())
        .WillByDefault(
            Return(IStepperMotor::Speed{test::MachineConfiguration::MotorSpeedDefault, Unit::Rpm}));
    ON_CALL(*m_mockStepperMotorFeeder, reset()).WillByDefault(Return(true));
    ON_CALL(*m_mockStepperMotorFeeder, getMaxSpeed())
        .WillByDefault(
            Return(IStepperMotor::Speed{test::MachineConfiguration::MotorSpeedDefault, Unit::Rpm}));
    ON_CALL(*m_mockTemperatureSensorFeeder, getTemperature())
        .WillByDefault(Return(ITemperatureSensor::Temperature{
            test::MachineConfiguration::DefaultTemperature, Unit::Celcius}));
    ON_CALL(*m_mockTemperatureSensorMerger, getTemperature())
        .WillByDefault(Return(ITemperatureSensor::Temperature{
            test::MachineConfiguration::DefaultTemperature, Unit::Celcius}));
}

void MachineApplicationIntegrationTest::prepareSwitchOn(bool expectHeating = true)
{
    if (expectHeating)
    {
        EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::High))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::High))
            .WillOnce(Return(true));
    }
    ON_CALL(*m_mockGpioPinSignalFilamentTensionLow, getState())
        .WillByDefault(Return(hal::IGpioPin::State::Low));
    ON_CALL(*m_mockGpioPinSignalFilamentTensionHigh, getState())
        .WillByDefault(Return(hal::IGpioPin::State::Low));
    ON_CALL(*m_mockGpioPinSignalFilamentTensionOverload, getState())
        .WillByDefault(Return(hal::IGpioPin::State::Low));
}

void MachineApplicationIntegrationTest::heatUp()
{
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::Low))
        .WillOnce(Return(true));
    EXPECT_NOTIFICATION_SUBSCRIBE(IFilamentPreHeater, NotificationTargetTemperatureRangeReached);
    ON_CALL(*m_mockTemperatureSensorFeeder, getTemperature())
        .WillByDefault(Return(ITemperatureSensor::Temperature{
            test::MachineConfiguration::HeaterTemperatureMax, Unit::Celcius}));
    EXPECT_NOTIFICATION(IFilamentPreHeater, NotificationTargetTemperatureRangeReached);

    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::Low))
        .WillOnce(Return(true));
    EXPECT_NOTIFICATION_SUBSCRIBE(IFilamentMergerHeater, NotificationTargetTemperatureRangeReached);
    ON_CALL(*m_mockTemperatureSensorMerger, getTemperature())
        .WillByDefault(Return(ITemperatureSensor::Temperature{
            test::MachineConfiguration::HeaterTemperatureMax, Unit::Celcius}));
    EXPECT_NOTIFICATION(IFilamentMergerHeater, NotificationTargetTemperatureRangeReached);
}

void MachineApplicationIntegrationTest::coolDown()
{
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::High))
        .WillOnce(Return(true));
    EXPECT_NOTIFICATION_SUBSCRIBE(IFilamentPreHeater, NotificationTargetTemperatureRangeLeft);
    ON_CALL(*m_mockTemperatureSensorFeeder, getTemperature())
        .WillByDefault(Return(ITemperatureSensor::Temperature{
            test::MachineConfiguration::HeaterTemperatureMin, Unit::Celcius}));
    EXPECT_NOTIFICATION(IFilamentPreHeater, NotificationTargetTemperatureRangeLeft);

    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::High))
        .WillOnce(Return(true));
    EXPECT_NOTIFICATION_SUBSCRIBE(IFilamentMergerHeater, NotificationTargetTemperatureRangeLeft);
    ON_CALL(*m_mockTemperatureSensorMerger, getTemperature())
        .WillByDefault(Return(ITemperatureSensor::Temperature{
            test::MachineConfiguration::HeaterTemperatureMin, Unit::Celcius}));
    EXPECT_NOTIFICATION(IFilamentMergerHeater, NotificationTargetTemperatureRangeLeft);
}

void MachineApplicationIntegrationTest::switchOnMachine()
{
    prepareSwitchOn();
    EXPECT_NOTIFICATION_SUBSCRIBE(IMachineControl, NotificationStopped);
    EXPECT_NOTIFICATION_SUBSCRIBE(IMachineControl, NotificationHeatingUp);
    send(IMachineControl::CommandRequestSwitchOn);
    EXPECT_NOTIFICATION(IMachineControl, NotificationStopped);
    EXPECT_STATE(IMachineControl, Stopped);
    send(IMachineControl::CommandRequestStart);
    EXPECT_NOTIFICATION(IMachineControl, NotificationHeatingUp);

    // Heating up
    EXPECT_CALL(*m_mockStepperMotorFeeder, rotate()).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorCoiler, rotate()).WillOnce(Return(true));
    EXPECT_NOTIFICATION_SUBSCRIBE(IMachineControl, NotificationRunning);
    heatUp();
    EXPECT_NOTIFICATION(IMachineControl, NotificationRunning);
}

void MachineApplicationIntegrationTest::switchOffMachine()
{
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::Low))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::Low))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorFeeder, stop(false)).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorCoiler, stop(false)).WillOnce(Return(true));
    EXPECT_NOTIFICATION_SUBSCRIBE(IMachineControl, NotificationSwitchedOff);
    send(IMachineControl::CommandRequestSwitchOff);
    EXPECT_NOTIFICATION(IMachineControl, NotificationSwitchedOff);
}
}  // namespace sugo

TEST_F(MachineApplicationIntegrationTest, StartExecutionGroup)
{
    switchOnMachine();
}

TEST_F(MachineApplicationIntegrationTest, StopExecutionGroup)
{
    switchOnMachine();
    switchOffMachine();
}

TEST_F(MachineApplicationIntegrationTest, HeaterCoolDown)
{
    switchOnMachine();

    // Starting cool down and heat up again
    coolDown();
    heatUp();
}

TEST_F(MachineApplicationIntegrationTest, MotorErrorOccurred)
{
    // switchOn
    prepareSwitchOn(false);
    EXPECT_NOTIFICATION_SUBSCRIBE(IMachineControl, NotificationStopped);
    send(IMachineControl::CommandRequestSwitchOn);
    EXPECT_NOTIFICATION(IMachineControl, NotificationStopped);
    EXPECT_STATE(IMachineControl, Stopped);

    // Simulate motor rotation error after heating up
    EXPECT_NOTIFICATION_SUBSCRIBE(IMachineControl, NotificationErrorOccurred);
    EXPECT_CALL(*m_mockStepperMotorFeeder, rotate()).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorCoiler, rotate()).WillOnce(Return(false));  // Error occurred!
    EXPECT_CALL(*m_mockStepperMotorFeeder, stop(false)).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorCoiler, stop(true)).WillOnce(Return(true));
    send(IMachineControl::CommandRequestStartHeatless);
    EXPECT_NOTIFICATION(IMachineControl, NotificationErrorOccurred);
    EXPECT_STATE(IMachineControl, Error);

    // Switch off
    EXPECT_NOTIFICATION_SUBSCRIBE(IMachineControl, NotificationSwitchedOff);
    send(IMachineControl::CommandRequestSwitchOff);
    EXPECT_NOTIFICATION(IMachineControl, NotificationSwitchedOff);
}

TEST_F(MachineApplicationIntegrationTest, TensionSensorTooLow)
{
    switchOnMachine();

    m_nextMotorSpeed = test::MachineConfiguration::MotorSpeedDefault +
                       test::MachineConfiguration::MotorSpeedIncrement;
    bool signalRisingEdge = true;
    EXPECT_NOTIFICATION_SUBSCRIBE(IFilamentTensionSensor, NotificationTensionTooLow);
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionLow, waitForEvent(_))
        .WillRepeatedly(Invoke([&](std::chrono::nanoseconds timeout) {
            if (signalRisingEdge)
            {
                signalRisingEdge = false;
                return IGpioPin::Event{std::chrono::nanoseconds(0),
                                       IGpioPin::EventType::RisingEdge};
            }
            else
            {
                std::this_thread::sleep_for(timeout);
                return IGpioPin::Event{std::chrono::nanoseconds(0), IGpioPin::EventType::Timeout};
            }
        }));
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionLow, getState())
        .WillRepeatedly(Return(hal::IGpioPin::State::High));
    EXPECT_CALL(*m_mockStepperMotorCoiler, setMaxSpeed(_))
        .WillRepeatedly([&](hal::IStepperMotor::Speed speed) {
            EXPECT_EQ(speed, m_nextMotorSpeed);
            m_nextMotorSpeed =
                std::clamp(m_nextMotorSpeed + test::MachineConfiguration::MotorSpeedIncrement, 0u,
                           test::MachineConfiguration::MotorSpeedMax);
        });
    EXPECT_NOTIFICATION(IFilamentTensionSensor, NotificationTensionTooLow);
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionLow, getState())
        .WillRepeatedly(Return(hal::IGpioPin::State::Low));
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(MachineApplicationIntegrationTest, TensionSensorTooHigh)
{
    switchOnMachine();

    m_nextMotorSpeed = test::MachineConfiguration::MotorSpeedDefault -
                       test::MachineConfiguration::MotorSpeedIncrement;
    bool signalRisingEdge = true;
    EXPECT_NOTIFICATION_SUBSCRIBE(IFilamentTensionSensor, NotificationTensionTooHigh);
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionHigh, waitForEvent(_))
        .WillRepeatedly(Invoke([&](std::chrono::nanoseconds timeout) {
            if (signalRisingEdge)
            {
                signalRisingEdge = false;
                return IGpioPin::Event{std::chrono::nanoseconds(0),
                                       IGpioPin::EventType::RisingEdge};
            }
            else
            {
                std::this_thread::sleep_for(timeout);
                return IGpioPin::Event{std::chrono::nanoseconds(0), IGpioPin::EventType::Timeout};
            }
        }));
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionHigh, getState())
        .WillRepeatedly(Return(hal::IGpioPin::State::High));
    EXPECT_CALL(*m_mockStepperMotorCoiler, setMaxSpeed(_))
        .WillRepeatedly([&](hal::IStepperMotor::Speed speed) {
            EXPECT_EQ(speed, m_nextMotorSpeed);
            m_nextMotorSpeed =
                std::clamp(m_nextMotorSpeed - test::MachineConfiguration::MotorSpeedIncrement, 0u,
                           test::MachineConfiguration::MotorSpeedMax);
        });
    EXPECT_NOTIFICATION(IFilamentTensionSensor, NotificationTensionTooHigh);
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionHigh, getState())
        .WillRepeatedly(Return(hal::IGpioPin::State::Low));
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_F(MachineApplicationIntegrationTest, TensionSensorOverloaded)
{
    switchOnMachine();

    m_nextMotorSpeed = test::MachineConfiguration::MotorSpeedDefault -
                       test::MachineConfiguration::MotorSpeedIncrement;
    EXPECT_NOTIFICATION_SUBSCRIBE(IFilamentTensionSensor, NotificationTensionOverloaded);
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionOverload, waitForEvent(_))
        .WillOnce(
            Return(IGpioPin::Event{std::chrono::nanoseconds(0), IGpioPin::EventType::RisingEdge}))
        .WillRepeatedly(Invoke([&](std::chrono::nanoseconds timeout) {
            std::this_thread::sleep_for(timeout);
            return IGpioPin::Event{std::chrono::nanoseconds(0), IGpioPin::EventType::Timeout};
        }));
    EXPECT_CALL(*m_mockStepperMotorCoiler, stop(false)).WillOnce(Return(true));
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionOverload, getState())
        .WillRepeatedly(Return(hal::IGpioPin::State::High));
    EXPECT_CALL(*m_mockStepperMotorCoiler, setMaxSpeed(_))
        .WillRepeatedly([&](hal::IStepperMotor::Speed speed) {
            EXPECT_EQ(speed, m_nextMotorSpeed);
            m_nextMotorSpeed =
                std::clamp(m_nextMotorSpeed - test::MachineConfiguration::MotorSpeedIncrement, 0u,
                           test::MachineConfiguration::MotorSpeedMax);
        });
    EXPECT_NOTIFICATION(IFilamentTensionSensor, NotificationTensionOverloaded);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_NOTIFICATION_SUBSCRIBE(IFilamentTensionSensor, NotificationTensionTooHigh);
    EXPECT_CALL(*m_mockStepperMotorCoiler, rotate()).WillOnce(Return(true));
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionOverload, waitForEvent(_))
        .WillOnce(
            Return(IGpioPin::Event{std::chrono::nanoseconds(0), IGpioPin::EventType::FallingEdge}))
        .WillRepeatedly(Invoke([&](std::chrono::nanoseconds timeout) {
            std::this_thread::sleep_for(timeout);
            return IGpioPin::Event{std::chrono::nanoseconds(0), IGpioPin::EventType::Timeout};
        }));
    EXPECT_CALL(*m_mockGpioPinSignalFilamentTensionOverload, getState())
        .WillRepeatedly(Return(hal::IGpioPin::State::Low));
    EXPECT_NOTIFICATION(IFilamentTensionSensor, NotificationTensionTooHigh);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}