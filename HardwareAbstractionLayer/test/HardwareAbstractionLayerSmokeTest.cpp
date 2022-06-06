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
    // Stepper motors
    for (auto& motorName : std::vector<std::string>{"feeder", "coiler"})
    {
        auto& stepperMotor = stepperMotorControl->getStepperMotorMap().at(motorName);
        EXPECT_TRUE(stepperMotor);
        EXPECT_EQ(stepperMotor->getMaxSpeed(), 100);
        EXPECT_EQ(stepperMotor->getPosition(), 0);
        EXPECT_TRUE(stepperMotor->rotateToPosition(100));
    }
}

TEST_F(HardwareAbstractionLayerSmokeTest, Test_AdcControl)
{
    HardwareAbstractionLayer hal;
    EXPECT_TRUE(hal.init(m_configuration));

    // ADC control
    auto& adcControl = hal.getAdcControllerMap().at("adc-control");
    EXPECT_TRUE(adcControl);
    // reference-2_5v
    auto& adcRef2_5v = adcControl->getAdcInputMap().at("reference-2_5v");
    EXPECT_TRUE(adcRef2_5v);
    const auto rawValueRef2_5v = adcRef2_5v->getRawValue();
    EXPECT_GT(rawValueRef2_5v, 0);
    const auto valueRef2_5v = adcRef2_5v->getUnitValue();
    EXPECT_EQ(valueRef2_5v.getUnit(), Unit::Volt);
    // reference-5v
    auto& adcRef5v = adcControl->getAdcInputMap().at("reference-5v");
    EXPECT_TRUE(adcRef5v);
    const auto rawValueRef5v = adcRef5v->getRawValue();
    EXPECT_GT(rawValueRef5v, 0);
    const auto valueRef5v = adcRef5v->getUnitValue();
    EXPECT_EQ(valueRef5v.getUnit(), Unit::Volt);
    // reference-0v
    auto& adcRef0v = adcControl->getAdcInputMap().at("reference-0v");
    EXPECT_TRUE(adcRef0v);
    const auto rawValueRef0v = adcRef0v->getRawValue();
    EXPECT_GT(rawValueRef0v, 0);
    const auto valueRef0v = adcRef0v->getUnitValue();
    EXPECT_EQ(valueRef0v.getUnit(), Unit::Volt);
    // ADC temperature channels
    for (auto& adcName : std::vector<std::string>{"temperature-feeder", "temperature-merger"})
    {
        // temperature-feeder
        auto& adc = adcControl->getAdcInputMap().at(adcName);
        EXPECT_TRUE(adc);
        for (unsigned i = 0; i < 10; i++)
        {
            const auto rawValue = adc->getRawValue();
            EXPECT_GT(rawValue, 0);
            std::cout << adcName << ": " << rawValue << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        const auto valueTemp = adc->getUnitValue();
        std::cout << adcName << ": " << valueTemp << std::endl;
        EXPECT_EQ(valueTemp.getUnit(), Unit::Celcius);
        EXPECT_GT(valueTemp.getValue(), 15);
        EXPECT_LT(valueTemp.getValue(), 30);
    }
}
