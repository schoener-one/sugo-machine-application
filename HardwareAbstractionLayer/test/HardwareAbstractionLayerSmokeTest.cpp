///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-30
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <sstream>
#include <string>
#include <thread>

#include "Configuration.hpp"
#include "ConfigurationFileParser.hpp"
#include "HardwareAbstractionLayer.hpp"
#include "IGpioControl.hpp"
#include "IGpioPin.hpp"
#include "ITemperatureSensor.hpp"
#include "ITemperatureSensorControl.hpp"
#include "Logger.hpp"

using namespace sugo::hal;
using namespace sugo;

class HardwareAbstractionLayerSmokeTest : public ::testing::Test
{
protected:
    HardwareAbstractionLayerSmokeTest()
    {
    }

    ~HardwareAbstractionLayerSmokeTest() override
    {
    }

    static void SetUpTestCase()
    {
        Logger::init();
    }

    void SetUp() override
    {
        std::ifstream inStream(m_configFile);
        ASSERT_TRUE(inStream.is_open());
        ConfigurationFileParser parser(inStream);

        HardwareAbstractionLayer::setConfiguration(m_configuration);
        parser.add(m_configuration);

        ASSERT_TRUE(parser.parse());
        ASSERT_TRUE(m_hal.init(m_configuration));
    }

    void TearDown() override
    {
    }

    constexpr static char m_configFile[] = "HardwareAbstractionLayer/conf/MachineConfig.json";

    Configuration            m_configuration;
    HardwareAbstractionLayer m_hal;
};

TEST_F(HardwareAbstractionLayerSmokeTest, Test_GpioControl)
{
    // GPIO control
    auto& gpioControl = m_hal.getGpioControllerMap().at("gpio-control");
    EXPECT_TRUE(gpioControl);
    // GPIO in-pins
    for (const std::string pin :
         {"signal-button-start", "signal-button-stop", "signal-filament-tension-low",
          "signal-filament-tension-high", "motor-control-error"})
    {
        std::cout << "GPIO IN pin: " << pin << std::endl;
        auto& gpioInPin = gpioControl->getGpioPinMap().at(pin);
        EXPECT_TRUE(gpioInPin);
        EXPECT_EQ(gpioInPin->getDirection(), IGpioPin::Direction::In);
        std::cout << "GPIO IN pin state: " << gpioInPin->getState() << std::endl;
    }
    // GPIO out-pins
    for (const std::string pin :
         {"relay-switch-fan-feeder", "relay-switch-fan-merger", "relay-switch-heater-feeder",
          "relay-switch-heater-merger", "relay-switch-light-power", "relay-switch-light-run",
          "relay-switch-light-ready"})
    {
        std::cout << "GPIO OUT pin: " << pin << std::endl;
        auto& gpioOutPin = gpioControl->getGpioPinMap().at(pin);
        EXPECT_TRUE(gpioOutPin);
        EXPECT_EQ(gpioOutPin->getDirection(), IGpioPin::Direction::Out);
        auto gpioState = gpioOutPin->getState();
        if (pin == "relay-switch-light-power")
        {
            EXPECT_EQ(gpioState, IGpioPin::State::High);
        }
        else
        {
            EXPECT_EQ(gpioState, IGpioPin::State::Low);
        }
        IGpioPin::State nextState =
            gpioState == IGpioPin::State::Low ? IGpioPin::State::High : IGpioPin::State::Low;
        EXPECT_TRUE(gpioOutPin->setState(nextState));
        EXPECT_EQ(nextState, gpioOutPin->getState());
        std::this_thread::sleep_for(std::chrono::seconds(2));
        nextState =
            gpioState == IGpioPin::State::Low ? IGpioPin::State::Low : IGpioPin::State::High;
        EXPECT_TRUE(gpioOutPin->setState(nextState));
    }
}

TEST_F(HardwareAbstractionLayerSmokeTest, Test_MotorControl)
{
    // Stepper motor control
    auto& stepperMotorControl = m_hal.getStepperMotorControllerMap().at("stepper-motor-control");
    EXPECT_TRUE(stepperMotorControl);
    const unsigned    maxSpeed      = 100u;
    constexpr int32_t stepsPerRound = 1600;

    // Stepper motors
    for (const std::string motorName : {"feeder", "coiler"})
    {
        std::cout << "Stepper motor: " << motorName << std::endl;
        auto& stepperMotor = stepperMotorControl->getStepperMotorMap().at(motorName);
        EXPECT_TRUE(stepperMotor);

        EXPECT_EQ(stepperMotor->getStepsPerRound(), stepsPerRound);
        EXPECT_EQ(stepperMotor->getMaxSpeed(), maxSpeed);
        EXPECT_EQ(stepperMotor->getPosition(), 0);
        std::cout << "rotate to: " << stepsPerRound << std::endl;
        EXPECT_TRUE(stepperMotor->rotateToPosition(stepsPerRound));
        EXPECT_EQ(stepperMotor->getPosition(), stepsPerRound);
        std::cout << "rotate to: " << stepsPerRound << std::endl;
        EXPECT_TRUE(stepperMotor->rotateToPosition(stepsPerRound));
        EXPECT_EQ(stepperMotor->getPosition(), stepsPerRound);
        std::cout << "rotate to: " << (stepsPerRound / 2) << std::endl;
        EXPECT_TRUE(stepperMotor->rotateToPosition(stepsPerRound / 2));
        EXPECT_EQ(stepperMotor->getPosition(), stepsPerRound / 2);
        std::cout << "rotate to: " << (0) << std::endl;
        EXPECT_TRUE(stepperMotor->rotateToPosition(0));
        EXPECT_EQ(stepperMotor->getPosition(), 0);
        std::cout << "rotate to: " << (stepsPerRound * -2) << std::endl;
        EXPECT_TRUE(stepperMotor->rotateToPosition(stepsPerRound * -2));
        EXPECT_EQ(stepperMotor->getPosition(), stepsPerRound * -2);

        EXPECT_TRUE(stepperMotor->rotate(IStepperMotor::Direction::Forward));
        std::this_thread::sleep_for(std::chrono::seconds(10));
        stepperMotor->getPosition();
        EXPECT_NEAR(stepperMotor->getSpeed().getValue(), maxSpeed, 2);
        EXPECT_TRUE(stepperMotor->stop(false));
        std::this_thread::sleep_for(std::chrono::seconds(1));
        EXPECT_EQ(stepperMotor->getSpeed().getValue(), 0);

        EXPECT_TRUE(stepperMotor->rotate(IStepperMotor::Direction::Backward));
        std::this_thread::sleep_for(std::chrono::seconds(10));
        EXPECT_NEAR(stepperMotor->getSpeed().getValue(), maxSpeed * -1, 2);
        EXPECT_TRUE(stepperMotor->stop(false));
        std::this_thread::sleep_for(std::chrono::seconds(1));
        EXPECT_EQ(stepperMotor->getSpeed().getValue(), 0);
    }
}

TEST_F(HardwareAbstractionLayerSmokeTest, Test_TemperatureSensorControl)
{
    auto& tempSensorControl =
        m_hal.getTemperatureSensorControllerMap().at("temperature-sensor-control");
    EXPECT_TRUE(tempSensorControl);

    for (const std::string sensorName : {"temperature-sensor-feeder", "temperature-sensor-merger"})
    {
        auto& tempSensor = tempSensorControl->getTemperatureSensorMap().at(sensorName);
        EXPECT_TRUE(tempSensor);
        const auto temperature = tempSensor->getTemperature();
        EXPECT_EQ(temperature.getUnit(), Unit::Celcius);
        EXPECT_NEAR(temperature.getValue(), 25, 10);
    }
}