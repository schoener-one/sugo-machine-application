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
#include "IAdcControl.hpp"
#include "IAdcFilter.hpp"
#include "IAdcInput.hpp"
#include "IGpioControl.hpp"
#include "IGpioPin.hpp"
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

        IHardwareAbstractionLayer::setConfiguration(m_configuration);
        parser.add(m_configuration);

        ASSERT_TRUE(parser.parse());
    }

    void TearDown() override
    {
    }

    constexpr static char m_configFile[] = "HardwareAbstractionLayer/conf/MachineConfig.json";

    Configuration m_configuration;
};

TEST_F(HardwareAbstractionLayerSmokeTest, Test_GpioControl)
{
    HardwareAbstractionLayer hal;
    EXPECT_TRUE(hal.init(m_configuration));

    // GPIO control
    auto& gpioControl = hal.getGpioControllerMap().at("gpio-control");
    EXPECT_TRUE(gpioControl);
    // GPIO in-pins
    for (auto& pin : std::vector<std::string>{
             "signal-button-start", "signal-button-stop", "signal-filament-tension-low",
             "signal-filament-tension-high", "adc-control-data-ready", "motor-control-error"})
    {
        std::cout << "GPIO IN pin: " << pin << std::endl;
        auto& gpioInPin = gpioControl->getGpioPinMap().at(pin);
        EXPECT_TRUE(gpioInPin);
        EXPECT_EQ(gpioInPin->getDirection(), IGpioPin::Direction::In);
        std::cout << "GPIO IN pin state: " << gpioInPin->getState() << std::endl;
    }
    // GPIO out-pins
    for (auto& pin : std::vector<std::string>{
             "relay-switch-heater-feeder", "relay-switch-heater-merger", "relay-switch-light-run",
             "relay-switch-light-on", "relay-switch-light-ready", "adc-control-chipselect",
             "adc-control-reset", "motor-control-reset"})
    {
        std::cout << "GPIO OUT pin: " << pin << std::endl;
        auto& gpioInPin = gpioControl->getGpioPinMap().at(pin);
        EXPECT_TRUE(gpioInPin);
        EXPECT_EQ(gpioInPin->getDirection(), IGpioPin::Direction::Out);
        auto gpioState = gpioInPin->getState();
        if ((pin == "relay-switch-light-on") || (pin == "motor-control-reset"))
        {
            EXPECT_EQ(gpioState, IGpioPin::State::High);
        }
        else
        {
            EXPECT_EQ(gpioState, IGpioPin::State::Low);
        }
    }
    auto& gpioSigLightRun = gpioControl->getGpioPinMap().at("relay-switch-light-run");
    EXPECT_TRUE(gpioSigLightRun);
    EXPECT_EQ(gpioSigLightRun->getDirection(), IGpioPin::Direction::Out);
    EXPECT_TRUE(gpioSigLightRun->setState(IGpioPin::State::High));
    EXPECT_TRUE(gpioSigLightRun->setState(IGpioPin::State::Low));
}

TEST_F(HardwareAbstractionLayerSmokeTest, Test_MotorControl)
{
    HardwareAbstractionLayer hal;
    EXPECT_TRUE(hal.init(m_configuration));
    // Stepper motor control
    auto& stepperMotorControl = hal.getStepperMotorControllerMap().at("stepper-motor-control");
    EXPECT_TRUE(stepperMotorControl);
    const unsigned    maxSpeed      = 100u;
    constexpr int32_t stepsPerRound = 1600;

    // Stepper motors
    for (auto& motorName : std::vector<std::string>{"feeder", "coiler"})
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
