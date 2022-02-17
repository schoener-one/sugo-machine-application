///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Configuration.hpp"
#include "Option.hpp"

using namespace sugo;
namespace po = boost::program_options;

class ConfigurationTest : public ::testing::Test
{
protected:
    ConfigurationTest() {}

    ~ConfigurationTest() override {}

    static void SetUpTestCase() {}

    void SetUp() override
    {
        m_config.add(Option("value.unsigned", 6u, "integer"));
        m_config.add(Option("value.boolhar", 'L', "char"));
        m_config.add(Option("value.bool", true, "boolean"));
    }

    void TearDown() override {}

    Configuration m_config;
};

TEST_F(ConfigurationTest, Configuration_AddOptions)
{
    Configuration::OptionDescriptions descriptions = m_config.getOptionDescriptions();
    const po::option_description*     p1 = descriptions.find_nothrow("value.unsigned", false);
    EXPECT_NE(p1, nullptr);
    const po::option_description* p2 = descriptions.find_nothrow("invalid", false);
    EXPECT_EQ(p2, nullptr);
}

TEST_F(ConfigurationTest, Configuration_GetValue)
{
    EXPECT_EQ(m_config["value.unsigned"].get<unsigned>(), 6);
    EXPECT_EQ(m_config["value.boolhar"].get<char>(), 'L');
    EXPECT_EQ(m_config["value.bool"].get<bool>(), true);
}

TEST_F(ConfigurationTest, Configuration_GetInvalidValue)
{
    EXPECT_THROW(m_config["invalid.value"].get<unsigned>(), std::out_of_range);
}
