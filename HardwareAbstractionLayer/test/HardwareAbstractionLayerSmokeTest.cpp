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

TEST_F(HardwareAbstractionLayerSmokeTest, HardwareAbstractionLayer_SmokeTest)
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

    // ADC control
    auto& adcControl = hal.getAdcControllerMap().at("adc-control");
    EXPECT_TRUE(adcControl);
    // test-resistance-100k
    auto& adcTestResistance100k = adcControl->getAdcInputMap().at("test-resistance-100k");
    EXPECT_TRUE(adcTestResistance100k);
    const auto rawValueTestResistance100k = adcTestResistance100k->getRawValue();
    EXPECT_GT(rawValueTestResistance100k, 0);
    const auto valueTestResistance100k = adcTestResistance100k->getUnitValue();
    EXPECT_EQ(valueTestResistance100k.getUnit(), Unit::Ohm);
    EXPECT_GT(valueTestResistance100k.getValue(), 90000);
    EXPECT_LT(valueTestResistance100k.getValue(), 110000);
    // test-resistance-0k
    auto& adcTestResistance0k = adcControl->getAdcInputMap().at("test-resistance-0k");
    EXPECT_TRUE(adcTestResistance0k);
    const auto rawValueTestResistance0k = adcTestResistance0k->getRawValue();
    EXPECT_GT(rawValueTestResistance0k, 0);
    const auto valueTestResistance0k = adcTestResistance0k->getUnitValue();
    EXPECT_EQ(valueTestResistance0k.getUnit(), Unit::Ohm);
    // ADC temperature channels
    for (auto& adcName : std::vector<std::string>{"temperature-feeder", "temperature-merger"})
    {
        // temperature-feeder
        auto& adc = adcControl->getAdcInputMap().at(adcName);
        EXPECT_TRUE(adc);
        const auto rawValueTempFeeder = adc->getRawValue();
        std::cout << adcName << ": " << rawValueTempFeeder << std::endl;
        EXPECT_GT(rawValueTempFeeder, 0);
        const auto valueTempFeeder = adc->getUnitValue();
        std::cout << adcName << ": " << valueTempFeeder << std::endl;
        EXPECT_EQ(valueTempFeeder.getUnit(), Unit::Celcius);
        EXPECT_GT(valueTempFeeder.getValue(), 15);
        EXPECT_LT(valueTempFeeder.getValue(), 30);
    }

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
