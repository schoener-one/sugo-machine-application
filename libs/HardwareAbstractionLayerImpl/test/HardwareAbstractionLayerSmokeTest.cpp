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

#include <chrono>
#include <sstream>
#include <string>
#include <thread>

#include "Common/Configuration.hpp"
#include "Common/ConfigurationFileParser.hpp"
#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/Configuration.hpp"
#include "HardwareAbstractionLayer/HardwareAbstractionLayer.hpp"
#include "HardwareAbstractionLayer/IGpioControl.hpp"
#include "HardwareAbstractionLayer/IGpioPin.hpp"
#include "HardwareAbstractionLayer/ITemperatureSensor.hpp"
#include "HardwareAbstractionLayer/ITemperatureSensorControl.hpp"

using namespace sugo::hal;
using namespace sugo;

class HardwareAbstractionLayerSmokeTest : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        common::Logger::init();
    }

    void SetUp() override
    {
        std::ifstream inStream(m_configFile);
        ASSERT_TRUE(inStream.is_open());
        common::ConfigurationFileParser parser(inStream);

        config::addConfigurationOptions(m_configuration);
        parser.add(m_configuration);

        ASSERT_TRUE(parser.parse());
        ASSERT_TRUE(m_hal.init(m_configuration));
    }

    void TestRotationTo(IStepperMotor& stepperMotor, const int32_t position)
    {
        std::cout << "Stepper motor " << stepperMotor.getId() << " rotates to: " << position
                  << std::endl;
        EXPECT_TRUE(stepperMotor.rotateToPosition(position));
        EXPECT_EQ(stepperMotor.getPosition(), position);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void TestRotate(IStepperMotor& stepperMotor, const IStepperMotor::Direction direction,
                    const unsigned maxSpeed)
    {
        stepperMotor.setMaxSpeed({maxSpeed, Unit::Rpm});
        std::cout << "Stepper motor " << stepperMotor.getId()
                  << " rotated in direction: " << direction << std::endl;
        EXPECT_TRUE(stepperMotor.rotate(direction));
        std::this_thread::sleep_for(std::chrono::seconds(10));
        EXPECT_NEAR(stepperMotor.getSpeed().getValue(), maxSpeed, 2u);
        EXPECT_TRUE(stepperMotor.stop(false));
        std::this_thread::sleep_for(std::chrono::seconds(2));
        EXPECT_EQ(stepperMotor.getSpeed().getValue(), 0);
    }
    constexpr static char m_configFile[] = "HardwareAbstractionLayerSmokeTestConfig.json";

    common::Configuration    m_configuration;
    HardwareAbstractionLayer m_hal;
};

TEST_F(HardwareAbstractionLayerSmokeTest, Test_GpioControl)
{
    // GPIO control
    auto& gpioControl = m_hal.getGpioControllerMap().at(id::GpioControl);
    EXPECT_TRUE(gpioControl);

    // GPIO in-pins
    for (const std::string pin : {
             id::GpioPinSignalFilamentTensionOverload, id::GpioPinSignalButtonStop,
             id::GpioPinSignalFilamentTensionLow, id::GpioPinSignalFilamentTensionHigh,
             //   id::GpioPinMotorControlError
         })
    {
        std::cout << "IO in pin: " << pin << std::endl;
        auto& gpioInPin = gpioControl->getGpioPinMap().at(pin);
        EXPECT_TRUE(gpioInPin);
        EXPECT_EQ(gpioInPin->getDirection(), IGpioPin::Direction::In);
        std::cout << "IO in pin state: " << gpioInPin->getState() << std::endl;
    }

    // GPIO out-pins
    for (const std::string pin :
         {id::GpioPinRelaySwitchFanFeeder, id::GpioPinRelaySwitchFanMerger,
          id::GpioPinRelaySwitchHeaterFeeder, id::GpioPinRelaySwitchHeaterMerger,
          id::GpioPinRelaySwitchLightPower, id::GpioPinRelaySwitchLightRun,
          id::GpioPinRelaySwitchLightReady, id::GpioPinTemperatureSensorControlCsFeeder,
          id::GpioPinTemperatureSensorControlCsMerger, id::GpioPinMotorControlReset})
    {
        std::cout << "IO out pin: " << pin << std::endl;
        auto& gpioOutPin = gpioControl->getGpioPinMap().at(pin);
        EXPECT_TRUE(gpioOutPin);
        EXPECT_EQ(gpioOutPin->getDirection(), IGpioPin::Direction::Out);
        auto gpioState = gpioOutPin->getState();
        if (pin == id::GpioPinRelaySwitchLightPower or pin == id::GpioPinMotorControlReset)
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
    auto& stepperMotorControl = m_hal.getStepperMotorControllerMap().at(id::StepperMotorControl);
    EXPECT_TRUE(stepperMotorControl);
    const unsigned    defaultMaxSpeed = 100u;
    const unsigned    maxSpeed        = 50u;
    constexpr int32_t stepsPerRound   = 1600;
    constexpr int64_t timePerStep     = 60000000LL / static_cast<int64_t>(maxSpeed * stepsPerRound);

    // Stepper motors
    for (const std::string motorName : {"feeder", "coiler"})
    {
        std::cout << "Stepper motor: " << motorName << std::endl;
        auto& stepperMotor = stepperMotorControl->getStepperMotorMap().at(motorName);
        EXPECT_TRUE(stepperMotor);

        EXPECT_EQ(stepperMotor->getStepsPerRound(), stepsPerRound);
        EXPECT_EQ(stepperMotor->getMaxSpeed(), defaultMaxSpeed);
        EXPECT_EQ(stepperMotor->getPosition(), 0);
        stepperMotor->setMaxSpeed({maxSpeed, Unit::Rpm});
        EXPECT_EQ(stepperMotor->getTimePerStep(maxSpeed).count(), timePerStep);

        TestRotationTo(*stepperMotor, stepsPerRound);
        TestRotationTo(*stepperMotor, stepsPerRound * 1 / 4);
        TestRotationTo(*stepperMotor, stepsPerRound * 2 / 4);
        TestRotationTo(*stepperMotor, stepsPerRound * 3 / 4);
        TestRotationTo(*stepperMotor, stepsPerRound * 2);
        TestRotationTo(*stepperMotor, stepsPerRound * 2 + 100);
        TestRotationTo(*stepperMotor, stepsPerRound * -2);

        TestRotate(*stepperMotor, IStepperMotor::Direction::Forward, maxSpeed);
        TestRotate(*stepperMotor, IStepperMotor::Direction::Forward, 80);
        TestRotate(*stepperMotor, IStepperMotor::Direction::Backward, maxSpeed);
        TestRotate(*stepperMotor, IStepperMotor::Direction::Backward, 80);
    }
}

TEST_F(HardwareAbstractionLayerSmokeTest, Test_TemperatureSensorControl)
{
    auto& tempSensorControl =
        m_hal.getTemperatureSensorControllerMap().at(id::TemperatureSensorControl);
    EXPECT_TRUE(tempSensorControl);

    for (const std::string sensorName : {id::TemperatureSensorFeeder, id::TemperatureSensorMerger})
    {
        auto& tempSensor = tempSensorControl->getTemperatureSensorMap().at(sensorName);
        EXPECT_TRUE(tempSensor);
        const auto temperature = tempSensor->getTemperature();
        std::cout << "Sensor " << sensorName << " temperature: " << temperature << "°C"
                  << std::endl;
        EXPECT_EQ(temperature.getUnit(), Unit::Celcius);
        EXPECT_NEAR(temperature.getValue(), 25, 10);
    }
}