///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-04
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
#include "Logger.hpp"

using namespace sugo::hal;
using namespace sugo;

const static std::string s_halConfig =
    "{"
    "    \"hardware-abstraction-layer\": {"
    "        \"gpio-control\": {"
    "            \"device\": \"gpiochip0\","
    "            \"gpio-pin-enabled\": ["
    "                \"relay-switch-fan-feeder\","
    "                \"relay-switch-fan-merger\","
    "                \"relay-switch-heater-feeder\","
    "                \"relay-switch-heater-merger\","
    "                \"relay-switch-light-run\","
    "                \"relay-switch-light-power\","
    "                \"relay-switch-light-ready\","
    "                \"signal-button-start\","
    "                \"signal-button-stop\","
    "                \"signal-filament-tension-low\","
    "                \"signal-filament-tension-high\","
    "                \"temperature-sensor-control-cs-feeder\","
    "                \"temperature-sensor-control-cs-merger\","
    "                \"motor-control-error\","
    "                \"motor-control-reset\""
    "            ],"
    "            \"gpio-pin\": {"
    "                \"relay-switch-heater-merger\": {"
    "                    \"pin\": 4,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"signal-button-start\": {"
    "                    \"pin\": 5,"
    "                    \"direction\": \"in\","
    "                    \"active-high\": false"
    "                },"
    "                \"signal-button-stop\": {"
    "                    \"pin\": 6,"
    "                    \"direction\": \"in\","
    "                    \"active-high\": false"
    "                },"
    "                \"signal-filament-tension-low\": {"
    "                    \"pin\": 12,"
    "                    \"direction\": \"in\""
    "                },"
    "                \"motor-control-error\": {"
    "                    \"pin\": 13,"
    "                    \"direction\": \"in\""
    "                },"
    "                \"signal-filament-tension-high\": {"
    "                    \"pin\": 16,"
    "                    \"direction\": \"in\""
    "                },"
    "                \"relay-switch-light-run\": {"
    "                    \"pin\": 17,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"relay-switch-light-ready\": {"
    "                    \"pin\": 18,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"motor-control-reset\": {"
    "                    \"pin\": 19,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"relay-switch-fan-merger\": {"
    "                    \"pin\": 20,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"relay-switch-heater-feeder\": {"
    "                    \"pin\": 21,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"temperature-sensor-control-cs-merger\": {"
    "                    \"pin\": 22,"
    "                    \"direction\": \"out\","
    "                    \"active-high\": false,"
    "                    \"state\": \"low\""
    "                },"
    "                \"relay-switch-light-power\": {"
    "                    \"pin\": 24,"
    "                    \"direction\": \"out\","
    "                    \"state\": \"high\""
    "                },"
    "                \"relay-switch-fan-feeder\": {"
    "                    \"pin\": 26,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"temperature-sensor-control-cs-feeder\": {"
    "                    \"pin\": 27,"
    "                    \"direction\": \"out\","
    "                    \"active-high\": false,"
    "                    \"state\": \"low\""
    "                }"
    "            }"
    "        },"
    "        \"stepper-motor-control\": {"
    "            \"device\": \"i2c-3\","
    "            \"motor-enabled\": ["
    "                \"feeder\","
    "                \"coiler\""
    "            ],"
    "            \"motor\": {"
    "                \"feeder\": {"
    "                    \"i2c-address\": 15,"
    "                    \"max-speed-rpm\": \"100\""
    "                },"
    "                \"coiler\": {"
    "                    \"i2c-address\": 14,"
    "                    \"max-speed-rpm\": \"100\""
    "                }"
    "            }"
    "        },"
    "        \"temperature-sensor-control\": {"
    "            \"device\": \"spidev0.0\","
    "            \"temperature-sensor-enabled\": ["
    "                \"temperature-sensor-feeder\","
    "                \"temperature-sensor-merger\""
    "            ],"
    "            \"temperature-sensor\": {"
    "                \"temperature-sensor-feeder\": {"
    "                    \"chip-select\": \"temperature-sensor-control-cs-feeder\""
    "                },"
    "                \"temperature-sensor-merger\": {"
    "                    \"chip-select\": \"temperature-sensor-control-cs-merger\""
    "                }"
    "            }"
    "        }"
    "    }"
    "}";

class HardwareAbstractionLayerTest : public ::testing::Test
{
protected:
    HardwareAbstractionLayerTest()
    {
    }

    ~HardwareAbstractionLayerTest() override
    {
    }

    static void SetUpTestCase()
    {
        Logger::init();
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(HardwareAbstractionLayerTest, IHardwareAbstractionLayer_SetConfiguration)
{
    std::istringstream      iss(s_halConfig);
    ConfigurationFileParser parser(iss);
    Configuration           config;
    HardwareAbstractionLayer::addConfigurationOptions(config);
    parser.add(config);
    EXPECT_TRUE(parser.parse());
    const std::vector<std::string>& pinEnabled =
        config.getOption("hardware-abstraction-layer.gpio-control.gpio-pin-enabled")
            .get<const std::vector<std::string>&>();
    EXPECT_GT(pinEnabled.size(), 0);
    unsigned pinValue =
        config
            .getOption(
                "hardware-abstraction-layer.gpio-control.gpio-pin.relay-switch-light-run.pin")
            .get<unsigned>();
    EXPECT_EQ(pinValue, 17);
    const std::vector<std::string>& tempSensorEnabled =
        config.getOption("hardware-abstraction-layer.temperature-sensor-control.temperature-sensor-enabled")
            .get<const std::vector<std::string>&>();
    EXPECT_EQ(tempSensorEnabled.size(), 2);
    const std::vector<std::string>& motorEnabled =
        config.getOption("hardware-abstraction-layer.stepper-motor-control.motor-enabled")
            .get<const std::vector<std::string>&>();
    EXPECT_EQ(motorEnabled.size(), 2);
    unsigned idValue =
        config
            .getOption("hardware-abstraction-layer.stepper-motor-control.motor.feeder.i2c-address")
            .get<unsigned>();
    EXPECT_EQ(idValue, 15);
    HardwareAbstractionLayer hal;
    EXPECT_TRUE(hal.init(config));
}
