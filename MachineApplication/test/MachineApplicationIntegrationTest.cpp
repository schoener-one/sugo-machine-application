//////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-08-03
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "CommandMessageBroker.hpp"
#include "Globals.hpp"
#include "IFilamentCoilControl.hpp"
#include "IFilamentCoilMotor.hpp"
#include "IFilamentMergerHeater.hpp"
#include "IFilamentPreHeater.hpp"
#include "IGpioControlMock.hpp"
#include "IGpioPinMock.hpp"
#include "IHardwareAbstractionLayerMock.hpp"
#include "IMachineControl.hpp"
#include "IOContext.hpp"
#include "IStepperMotorControlMock.hpp"
#include "IStepperMotorMock.hpp"
#include "ITemperatureSensorControlMock.hpp"
#include "ITemperatureSensorMock.hpp"
#include "IntegrationTest.hpp"
#include "Logger.hpp"
#include "MachineConfig.hpp"
#include "RemoteControlServer.hpp"
#include "ServiceComponentsExecutionGroup.hpp"
#include "ServiceLocator.hpp"
#include "Thread.hpp"
#include "UserInterfaceControl.hpp"

using namespace sugo;
using namespace sugo::message;
using namespace hal;
using namespace remote_control;

using ::testing::_;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

namespace sugo
{
class MachineApplicationIntegrationTest : public IntegrationTest
{
protected:
    inline static const message::Address ReceiverId{"MachineApplicationIntegrationTest"};

    MachineApplicationIntegrationTest() : IntegrationTest(ReceiverId)
    {
    }

    void SetUp() override
    {
        IntegrationTest::SetUp();

        prepareForStartComponents();
        ASSERT_TRUE(m_components.start(m_serviceLocator));

        auto it = std::find_if(
            m_components.getBundles().begin(), m_components.getBundles().end(),
            [](const auto& bundle) { return (bundle->getId() == "UserInterfaceControl"); });
        assert((it != m_components.getBundles().end()) && (*it));

        IServiceComponent&    serviceComponent = it->get()->getServiceComponent();
        UserInterfaceControl* userInterfaceControl =
            static_cast<UserInterfaceControl*>(&serviceComponent);
        userInterfaceControl->registerSendNotification(m_mockNotificationHandler.AsStdFunction());
    }

    void TearDown() override
    {
        // m_remoteControlServer->stop();
        m_components.stop();
        IntegrationTest::TearDown();
    }

    static void SetUpTestCase()
    {
        Logger::init(Logger::Severity::debug, ReceiverId);
    }

    void prepareHardwareAbstractionLayer();
    void prepareForStartComponents();
    void prepareSwitchOn();
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

    NiceMock<MockFunction<void(const Json&)>> m_mockNotificationHandler;

    NiceMock<IHardwareAbstractionLayerMock> m_mockHardwareAbstractionLayer;
    ServiceLocator                          m_serviceLocator;
    ServiceComponentsExecutionGroup         m_components;
    // std::shared_ptr<RemoteControlServer>    m_remoteControlServer;

    static constexpr unsigned DefaultMotorSpeed  = 100;
    static constexpr int32_t  DefaultTemperature = 25;
};

void MachineApplicationIntegrationTest::prepareHardwareAbstractionLayer()
{
    m_stepperMotorControllerMap.emplace(
        config::StepperMotorControlId,
        m_mockStepperMotorControl = std::make_shared<NiceMock<IStepperMotorControlMock>>());
    m_stepperMotorMap.emplace(
        config::StepperMotorCoilerId,
        m_mockStepperMotorCoiler = std::make_shared<NiceMock<IStepperMotorMock>>());
    m_stepperMotorMap.emplace(
        config::StepperMotorFeederId,
        m_mockStepperMotorFeeder = std::make_shared<NiceMock<IStepperMotorMock>>());

    m_temperatureSensorControllerMap.emplace(
        config::TemperatureSensorControlId,
        m_mockTemperatureSensorControl =
            std::make_shared<NiceMock<ITemperatureSensorControlMock>>());
    m_temperatureSensorMap.emplace(
        config::TemperatureSensorMergerId,
        m_mockTemperatureSensorMerger = std::make_shared<NiceMock<ITemperatureSensorMock>>());
    m_temperatureSensorMap.emplace(
        config::TemperatureSensorFeederId,
        m_mockTemperatureSensorFeeder = std::make_shared<NiceMock<ITemperatureSensorMock>>());

    m_gpioControllerMap.emplace(config::GpioControlId,
                                m_mockGpioControl = std::make_shared<NiceMock<IGpioControlMock>>());
    m_gpioPinMap.emplace(
        config::GpioPinRelaySwitchFeederHeaterId,
        m_mockGpioPinRelaySwitchHeaterFeeder = std::make_shared<NiceMock<IGpioPinMock>>());
    m_gpioPinMap.emplace(
        config::GpioPinRelaySwitchMergerHeaterId,
        m_mockGpioPinRelaySwitchHeaterMerger = std::make_shared<NiceMock<IGpioPinMock>>());
    m_gpioPinMap.emplace(
        config::GpioPinRelaySwitchLightRunId,
        m_mockGpioPinRelaySwitchLightRun = std::make_shared<NiceMock<IGpioPinMock>>());
    m_gpioPinMap.emplace(
        config::GpioPinRelaySwitchLightPowerId,
        m_mockGpioPinRelaySwitchLightPower = std::make_shared<NiceMock<IGpioPinMock>>());
    m_gpioPinMap.emplace(
        config::GpioPinRelaySwitchLightReadyId,
        m_mockGpioPinRelaySwitchLightReady = std::make_shared<NiceMock<IGpioPinMock>>());

    m_serviceLocator.add<IHardwareAbstractionLayer>(m_mockHardwareAbstractionLayer);
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
}

void MachineApplicationIntegrationTest::prepareForStartComponents()
{
    prepareHardwareAbstractionLayer();
    ON_CALL(*m_mockStepperMotorCoiler, reset()).WillByDefault(Return(true));
    ON_CALL(*m_mockStepperMotorCoiler, getMaxSpeed())
        .WillByDefault(Return(IStepperMotor::Speed{DefaultMotorSpeed, Unit::Rpm}));
    ON_CALL(*m_mockStepperMotorFeeder, reset()).WillByDefault(Return(true));
    ON_CALL(*m_mockStepperMotorFeeder, getMaxSpeed())
        .WillByDefault(Return(IStepperMotor::Speed{DefaultMotorSpeed, Unit::Rpm}));
    ON_CALL(*m_mockTemperatureSensorFeeder, getTemperature())
        .WillByDefault(Return(ITemperatureSensor::Temperature{DefaultTemperature, Unit::Celcius}));
    ON_CALL(*m_mockTemperatureSensorMerger, getTemperature())
        .WillByDefault(Return(ITemperatureSensor::Temperature{DefaultTemperature, Unit::Celcius}));
}

void MachineApplicationIntegrationTest::prepareSwitchOn()
{
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::High))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::High))
        .WillOnce(Return(true));
}

void MachineApplicationIntegrationTest::heatUp()
{
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::Low))
        .WillOnce(Return(true));
    EXPECT_NOTIFICATION_BEGIN(IFilamentPreHeater, NotificationTargetTemperatureRangeReached);
    ON_CALL(*m_mockTemperatureSensorFeeder, getTemperature())
        .WillByDefault(
            Return(ITemperatureSensor::Temperature{config::MaxHeaterTemperature, Unit::Celcius}));
    EXPECT_NOTIFICATION_END(IFilamentPreHeater, NotificationTargetTemperatureRangeReached);

    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::Low))
        .WillOnce(Return(true));
    EXPECT_NOTIFICATION_BEGIN(IFilamentMergerHeater, NotificationTargetTemperatureRangeReached);
    ON_CALL(*m_mockTemperatureSensorMerger, getTemperature())
        .WillByDefault(
            Return(ITemperatureSensor::Temperature{config::MaxHeaterTemperature, Unit::Celcius}));
    EXPECT_NOTIFICATION_END(IFilamentMergerHeater, NotificationTargetTemperatureRangeReached);
}

void MachineApplicationIntegrationTest::coolDown()
{
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::High))
        .WillOnce(Return(true));
    EXPECT_NOTIFICATION_BEGIN(IFilamentPreHeater, NotificationTargetTemperatureRangeLeft);
    ON_CALL(*m_mockTemperatureSensorFeeder, getTemperature())
        .WillByDefault(
            Return(ITemperatureSensor::Temperature{config::MinHeaterTemperature, Unit::Celcius}));
    EXPECT_NOTIFICATION_END(IFilamentPreHeater, NotificationTargetTemperatureRangeLeft);

    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::High))
        .WillOnce(Return(true));
    EXPECT_NOTIFICATION_BEGIN(IFilamentMergerHeater, NotificationTargetTemperatureRangeLeft);
    ON_CALL(*m_mockTemperatureSensorMerger, getTemperature())
        .WillByDefault(
            Return(ITemperatureSensor::Temperature{config::MinHeaterTemperature, Unit::Celcius}));
    EXPECT_NOTIFICATION_END(IFilamentMergerHeater, NotificationTargetTemperatureRangeLeft);
}

void MachineApplicationIntegrationTest::switchOnMachine()
{
    prepareSwitchOn();
    EXPECT_NOTIFICATION_BEGIN(IMachineControl, NotificationStarting);
    send(IMachineControl::CommandSwitchOn);
    EXPECT_NOTIFICATION_END(IMachineControl, NotificationStarting);
    EXPECT_STATE(IMachineControl, WaitingForHeatedUp);

    // Heating up
    EXPECT_CALL(*m_mockStepperMotorFeeder, rotate(_)).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorCoiler, rotate(_)).WillOnce(Return(true));
    EXPECT_NOTIFICATION_BEGIN(IMachineControl, NotificationRunning);
    heatUp();
    EXPECT_NOTIFICATION_END(IMachineControl, NotificationRunning);
}

void MachineApplicationIntegrationTest::switchOffMachine()
{
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::Low))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::Low))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorFeeder, stop(false)).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorCoiler, stop(false)).WillOnce(Return(true));
    EXPECT_NOTIFICATION_BEGIN(IMachineControl, NotificationSwitchedOff);
    send(IMachineControl::CommandSwitchOff);
    EXPECT_NOTIFICATION_END(IMachineControl, NotificationSwitchedOff);
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
    prepareSwitchOn();
    EXPECT_NOTIFICATION_BEGIN(IMachineControl, NotificationStarting);
    send(IMachineControl::CommandSwitchOn);
    EXPECT_NOTIFICATION_END(IMachineControl, NotificationStarting);
    EXPECT_STATE(IMachineControl, WaitingForHeatedUp);

    // Simulate motor rotation error after heating up
    EXPECT_NOTIFICATION_BEGIN(IMachineControl, NotificationErrorOccurred);
    EXPECT_CALL(*m_mockStepperMotorFeeder, rotate(_)).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorCoiler, rotate(_)).WillOnce(Return(false));  // Error occurred!
    EXPECT_CALL(*m_mockStepperMotorFeeder, stop(false)).WillOnce(Return(true));
    EXPECT_CALL(*m_mockStepperMotorCoiler, stop(true)).WillOnce(Return(true));

    // Heat up
    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterFeeder, setState(hal::IGpioPin::State::Low))
        .Times(2)
        .WillRepeatedly(Return(true));
    ON_CALL(*m_mockTemperatureSensorFeeder, getTemperature())
        .WillByDefault(
            Return(ITemperatureSensor::Temperature{config::MaxHeaterTemperature, Unit::Celcius}));

    EXPECT_CALL(*m_mockGpioPinRelaySwitchHeaterMerger, setState(hal::IGpioPin::State::Low))
        .Times(2)
        .WillRepeatedly(Return(true));
    ON_CALL(*m_mockTemperatureSensorMerger, getTemperature())
        .WillByDefault(
            Return(ITemperatureSensor::Temperature{config::MaxHeaterTemperature, Unit::Celcius}));

    EXPECT_NOTIFICATION_END(IMachineControl, NotificationErrorOccurred);
    EXPECT_STATE(IMachineControl, Error);

    // Switch off
    EXPECT_NOTIFICATION_BEGIN(IMachineControl, NotificationSwitchedOff);
    send(IMachineControl::CommandSwitchOff);
    EXPECT_NOTIFICATION_END(IMachineControl, NotificationSwitchedOff);
}

TEST_F(MachineApplicationIntegrationTest, TensionSensor)
{
    switchOnMachine();

}