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
#include "IGpioControl.hpp"
#include "IGpioPin.hpp"
#include "HardwareAbstractionLayer.hpp"
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
        std::ifstream           inStream(m_configFile);
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

    auto& gpioControlMap = hal.getGpioControllerMap();
    EXPECT_FALSE(gpioControlMap.empty());
    for (auto& itemGpioControl : gpioControlMap)
    {
        auto& gpioPinMap = itemGpioControl.second->getGpioPinMap();
        EXPECT_FALSE(gpioPinMap.empty());
        for (auto& itemGpio : gpioPinMap)
        {
            auto& gpio = itemGpio.second;
            LOG(info) << "GPIO pin '" << gpio->getId() << "' direction: " << gpio->getDirection();
            LOG(info) << "GPIO pin '" << gpio->getId() << "' state: " << gpio->getState();

            if (gpio->getDirection() == IGpioPin::Direction::Out)
            {
                gpio->setState(IGpioPin::State::High);
                EXPECT_EQ(gpio->getState(), IGpioPin::State::High);
            }
            else
            {
                EXPECT_EQ(gpio->getState(), IGpioPin::State::Low);
            }
        }
    }
}
