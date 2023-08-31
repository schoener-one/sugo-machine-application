///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2023
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Common/Configuration.hpp"
#include "Common/Option.hpp"

using namespace sugo::common;
namespace po = boost::program_options;

class ConfigurationTest : public ::testing::Test
{
protected:
    ConfigurationTest()
    {
    }

    ~ConfigurationTest() override
    {
    }

    static void SetUpTestCase()
    {
    }

    void SetUp() override
    {
        m_config.add(Option("value.unsigned", 6u, "integer"));
        m_config.add(Option("value.boolhar", 'L', "char"));
        m_config.add(Option("value.bool", true, "boolean"));
    }

    void TearDown() override
    {
    }

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
