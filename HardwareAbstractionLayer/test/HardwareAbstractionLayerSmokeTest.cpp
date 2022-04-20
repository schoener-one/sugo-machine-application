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
    auto& gpioSigBtStart = gpioControl->getGpioPinMap().at("signal-button-start");
    EXPECT_TRUE(gpioSigBtStart);
    EXPECT_EQ(gpioSigBtStart->getDirection(), IGpioPin::Direction::In);
    auto gpioState = gpioSigBtStart->getState();
    EXPECT_EQ(gpioState, IGpioPin::State::Low);
    auto& gpioSigLightRun = gpioControl->getGpioPinMap().at("relay-switch-light-run");
    EXPECT_TRUE(gpioSigLightRun);
    EXPECT_EQ(gpioSigLightRun->getDirection(), IGpioPin::Direction::Out);
    EXPECT_TRUE(gpioSigLightRun->setState(IGpioPin::State::High));
    EXPECT_TRUE(gpioSigLightRun->setState(IGpioPin::State::Low));

    // ADC control
    auto& adcControl = hal.getAdcControllerMap().at("adc-control");
    EXPECT_TRUE(adcControl);
    // temperature-feeder
    auto& adcTempFeeder = adcControl->getAdcInputMap().at("temperature-feeder");
    EXPECT_TRUE(adcTempFeeder);
    const auto rawValueTempFeeder = adcTempFeeder->getRawValue();
    EXPECT_GT(rawValueTempFeeder, 0);
    const auto valueTempFeeder = adcTempFeeder->getUnitValue();
    EXPECT_EQ(valueTempFeeder.getUnit(), Unit::Celcius);
    EXPECT_GT(valueTempFeeder.getValue(), 15);
    EXPECT_LT(valueTempFeeder.getValue(), 30);    
    // test-resistance-100k
    auto& adcTestResistance100k = adcControl->getAdcInputMap().at("test-resistance-100k");
    EXPECT_TRUE(adcTestResistance100k);
    const auto rawValueTestResistance100k = adcTestResistance100k->getRawValue();
    EXPECT_GT(rawValueTestResistance100k, 0);
    const auto valueTestResistance100k = adcTestResistance100k->getUnitValue();
    EXPECT_EQ(valueTestResistance100k.getUnit(), Unit::Ohm);
    EXPECT_GT(valueTestResistance100k.getValue(), 90000);
    EXPECT_LT(valueTestResistance100k.getValue(), 110000);    
    // test-resistance-none
    auto& adcTestResistanceNone = adcControl->getAdcInputMap().at("test-resistance-none");
    EXPECT_TRUE(adcTestResistanceNone);
    const auto rawValueTestResistanceNone = adcTestResistanceNone->getRawValue();
    EXPECT_GT(rawValueTestResistanceNone, 0);
    const auto valueTestResistanceNone = adcTestResistanceNone->getUnitValue();
    EXPECT_EQ(valueTestResistanceNone.getUnit(), Unit::Ohm);
}
