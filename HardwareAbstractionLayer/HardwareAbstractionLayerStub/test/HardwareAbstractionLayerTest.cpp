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
    "                \"relay-switch-heater-feeder\","
    "                \"relay-switch-heater-merger\","
    "                \"relay-switch-light-run\","
    "                \"relay-switch-light-on\","
    "                \"signal-button-start\","
    "                \"signal-button-stop\","
    "                \"adc-control-chipselect\","
    "                \"adc-control-data-ready\","
    "                \"adc-control-reset\","
    "                \"motor-control-error\","
    "                \"motor-control-reset\""
    "            ],"
    "            \"gpio-pin\": {"
    "                \"relay-switch-light-run\": {"
    "                    \"pin\": 4,"
    "                    \"direction\": \"out\","
    "                    \"state\": \"low\""
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
    "                \"motor-control-error\": {"
    "                    \"pin\": 13,"
    "                    \"direction\": \"in\""
    "                },"
    "                \"adc-control-data-ready\": {"
    "                    \"pin\": 17,"
    "                    \"direction\": \"in\","
    "                    \"active-high\": false"
    "                },"
    "                \"motor-control-reset\": {"
    "                    \"pin\": 19,"
    "                    \"direction\": \"out\","
    "                    \"active-high\": false"
    "                },"
    "                \"adc-control-reset\": {"
    "                    \"pin\": 18,"
    "                    \"direction\": \"out\","
    "                    \"active-high\": false"
    "                },"
    "                \"relay-switch-heater-merger\": {"
    "                    \"pin\": 20,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"relay-switch-light-on\": {"
    "                    \"pin\": 21,"
    "                    \"direction\": \"out\","
    "                    \"state\": \"high\""
    "                },"
    "                \"adc-control-chipselect\": {"
    "                    \"pin\": 22,"
    "                    \"direction\": \"out\","
    "                    \"active-high\": false,"
    "                    \"state\": \"high\""
    "                },"
    "                \"relay-switch-heater-feeder\": {"
    "                    \"pin\": 26,"
    "                    \"direction\": \"out\""
    "                }"
    "            }"
    "        },"
    "        \"stepper-motor-control\": {"
    "            \"device\": \"i2c-1\","
    "            \"motor-enabled\": ["
    "                \"feeder\","
    "                \"coiler\""
    "            ],"
    "            \"motor\": {"
    "                \"feeder\": {"
    "                    \"i2c-address\": 14,"
    "                    \"max-speed-rpm\": \"100\""
    "                },"
    "                \"coiler\": {"
    "                    \"i2c-address\": 15,"
    "                    \"max-speed-rpm\": \"100\""
    "                }"
    "            }"
    "        },"
    "        \"adc-control\": {"
    "            \"device\": \"spidev0.0\","
    "            \"adc-enabled\": ["
    "                \"temperature-feeder\","
    "                \"temperature-merger\","
    "                \"reference-2_5v\","
    "                \"reference-5v\","
    "                \"reference-0v\""
    "            ],"
    "            \"adc\": {"
    "                \"temperature-feeder\": {"
    "                    \"channel\": 0,"
    "                    \"adc-filter\": \"ntc-temperature\""
    "                },"
    "                \"temperature-merger\": {"
    "                    \"channel\": 1,"
    "                    \"adc-filter\": \"ntc-temperature\""
    "                },"
    "                \"reference-2_5v\": {"
    "                    \"channel\": 2,"
    "                    \"adc-filter\": \"voltage-divider\""
    "                },"
    "                \"reference-5v\": {"
    "                    \"channel\": 3,"
    "                    \"adc-filter\": \"voltage-divider\""
    "                }"
    "            },"
    "            \"adc-filter-enabled\": ["
    "                \"ntc-temperature\","
    "                \"voltage-divider\","
    "                \"voltage\""
    "            ],"
    "            \"adc-filter\": {"
    "                \"ntc-temperature\": {"
    "                    \"ref-voltage\": 4920,"
    "                    \"resistance\": 100000,"
    "                    \"divider-resistance\": 100000,"
    "                    \"translation\": \"ntc-100k-temperature\""
    "                },"
    "                \"voltage-divider\": {"
    "                    \"ref-voltage\": 4920,"
    "                    \"resistance\": 100000,"
    "                    \"divider-resistance\": 100000,"
    "                    \"translation\": \"voltage-divider-resistance\""
    "                },"
    "                \"voltage\": {"
    "                    \"ref-voltage\": 4920,"
    "                    \"translation\": \"voltage\""
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
    IHardwareAbstractionLayer::setConfiguration(config);
    parser.add(config);
    EXPECT_TRUE(parser.parse());
    const std::vector<std::string>& pinEnabled =
        config.getOption("hardware-abstraction-layer.gpio-control.gpio-pin-enabled")
            .get<const std::vector<std::string>&>();
    EXPECT_EQ(pinEnabled.size(), 11);
    unsigned pinValue =
        config.getOption("hardware-abstraction-layer.gpio-control.gpio-pin.adc-control-data-ready.pin")
            .get<unsigned>();
    EXPECT_EQ(pinValue, 17);
    const std::vector<std::string>& adcEnabled =
        config.getOption("hardware-abstraction-layer.adc-control.adc-enabled")
            .get<const std::vector<std::string>&>();
    EXPECT_EQ(adcEnabled.size(), 4);
    unsigned inputValue =
        config.getOption("hardware-abstraction-layer.adc-control.adc.temperature-merger.channel")
            .get<unsigned>();
    EXPECT_EQ(inputValue, 1);
    const std::vector<std::string>& motorEnabled =
        config.getOption("hardware-abstraction-layer.stepper-motor-control.motor-enabled")
            .get<const std::vector<std::string>&>();
    EXPECT_EQ(motorEnabled.size(), 2);
    unsigned idValue =
        config.getOption("hardware-abstraction-layer.stepper-motor-control.motor.feeder.i2c-address")
            .get<unsigned>();
    EXPECT_EQ(idValue, 14);
    HardwareAbstractionLayer hal;
    EXPECT_TRUE(hal.init(config));
}
