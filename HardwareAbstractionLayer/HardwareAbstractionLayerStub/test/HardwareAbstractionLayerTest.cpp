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
    "            \"pins\": "
    "               {"
    "                  \"adc-control-data-ready\": {"
    "                      \"pin\": 5,"
    "                      \"active-high\": false"
    "                  }"
    "               }"
    "           }"
    "       }"
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
    HardwareAbstractionLayer hal;
    EXPECT_TRUE(hal.init(config));
}
