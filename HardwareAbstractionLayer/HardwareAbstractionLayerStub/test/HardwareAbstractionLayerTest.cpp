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
    "            \"pin-enabled\": ["
    "                \"adc-control-data-ready\","
    "                \"adc-control-reset\""
    "            ],"
    "            \"pin\": {"
    "                \"adc-control-data-ready\": {"
    "                    \"pin\": 17,"
    "                    \"direction\": \"in\","
    "                    \"active-high\": false"
    "                },"
    "                \"adc-control-reset\": {"
    "                    \"pin\": 18,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"adc-control-chipselect\": {"
    "                    \"pin\": 22,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"relay-switch-heater-feeder\": {"
    "                    \"pin\": 26,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"relay-switch-heater-merger\": {"
    "                    \"pin\": 20,"
    "                    \"direction\": \"out\""
    "                },"
    "                \"relay-switch-ch3\": {"
    "                    \"pin\": 21,"
    "                    \"direction\": \"out\""
    "                }"
    "            }"
    "        },"
    "        \"stepper-motor-control\": {"
    "            \"motor-enabled\": ["
    "                \"feeder\","
    "                \"coiler\""
    "            ],"
    "            \"motor\": {"
    "                \"feeder\": {"
    "                    \"i2c-id\": 14,"
    "                    \"max-speed-rpm\": \"100\""
    "                },"
    "                \"coiler\": {"
    "                    \"i2c-id\": 15,"
    "                    \"max-speed-rpm\": \"100\""
    "                }"
    "            }"
    "        },"
    "        \"adc-control\": {"
    "            \"adc-enabled\": ["
    "                \"temperature-feeder\","
    "                \"temperature-merger\","
    "                \"temperature-hot-filament\","
    "                \"temperature-cool-filament\","
    "                \"tension-filament\""
    "            ],"
    "            \"adc\": {"
    "                \"temperature-feeder\": {"
    "                    \"input\": 0,"
    "                    \"filter\": \"ntc-celcius\""
    "                },"
    "                \"temperature-merger\": {"
    "                    \"input\": 1,"
    "                    \"filter\": \"ntc-celcius\""
    "                },"
    "                \"temperature-hot-filament\": {"
    "                    \"input\": 2,"
    "                    \"filter\": \"ntc-celcius\""
    "                },"
    "                \"temperature-cool-filament\": {"
    "                    \"input\": 3,"
    "                    \"filter\": \"ntc-celcius\""
    "                },"
    "                \"tension-filament\": {"
    "                    \"input\": 4,"
    "                    \"filter\": \"pot-degree\""
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
    EXPECT_EQ(pinEnabled.size(), 2);
    unsigned pinValue =
        config.getOption("hardware-abstraction-layer.gpio-control.pin.adc-control-data-ready.pin")
            .get<unsigned>();
    EXPECT_EQ(pinValue, 17);
    const std::vector<std::string>& adcEnabled =
        config.getOption("hardware-abstraction-layer.adc-control.adc-enabled")
            .get<const std::vector<std::string>&>();
    EXPECT_EQ(adcEnabled.size(), 5);
    unsigned inputValue =
        config.getOption("hardware-abstraction-layer.adc-control.adc.temperature-cool-filament.input")
            .get<unsigned>();
    EXPECT_EQ(inputValue, 3);
    const std::vector<std::string>& motorEnabled =
        config.getOption("hardware-abstraction-layer.stepper-motor-control.motor-enabled")
            .get<const std::vector<std::string>&>();
    EXPECT_EQ(motorEnabled.size(), 2);
    unsigned idValue =
        config.getOption("hardware-abstraction-layer.stepper-motor-control.motor.feeder.i2c-id")
            .get<unsigned>();
    EXPECT_EQ(idValue, 14);
    HardwareAbstractionLayer hal;
    EXPECT_TRUE(hal.init(config));
}
