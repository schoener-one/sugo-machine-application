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

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Common/ConfigurationFileParser.hpp"
#include "Common/IConfigurationMock.hpp"
#include "Common/Logger.hpp"

namespace po = boost::program_options;
using namespace sugo::common;

class ConfigurationFileParserTest : public ::testing::Test
{
protected:
    ConfigurationFileParserTest()
    {
    }

    ~ConfigurationFileParserTest() override
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

    IConfigurationMock m_mockConfig;
};

TEST_F(ConfigurationFileParserTest, ConfigurationFileParser_Parse)
{
    std::istringstream iss(
        "{"
        "  \"Module1\": {"
        "    \"Option1\": 4,"
        "    \"Option2\": \"Foo\""
        "  },"
        "  \"Module2\": {"
        "     \"Option3\": 0.23"
        "  }"
        "}");
    ConfigurationFileParser parser(iss);
    parser.add(m_mockConfig);
    IConfiguration::OptionDescriptions descriptions;
    // clang-format off
    descriptions.add_options()
		("Module1.Option1", po::value<unsigned>()->default_value(0), "this is Option1")
		("Module1.Option2", po::value<std::string>()->default_value(""), "this is Option2")
		("Module2.Option3", po::value<double>()->default_value(0.0), "this is Option3");
    // clang-format on
    EXPECT_CALL(m_mockConfig, getOptionDescriptions).WillOnce(testing::Return(descriptions));
    EXPECT_CALL(m_mockConfig, set).WillOnce(testing::Return(true));
    const bool success = parser.parse();
    EXPECT_TRUE(success);
}

TEST_F(ConfigurationFileParserTest, ConfigurationFileParser_ParseInvalidConfiguration)
{
    std::istringstream iss(
        "{"
        "  \"Module1\": {"
        "    \"Option1\": 4,"
        "    \"Option2\": \"Foo\""
        "  },"
        "  \"Module2\": {"
        "     \"InvalidOption3\": 0.23"
        "  }"
        "}");
    ConfigurationFileParser parser(iss);
    parser.add(m_mockConfig);
    IConfiguration::OptionDescriptions descriptions;
    // clang-format off
    descriptions.add_options()
		("Module1.Option1", po::value<unsigned>()->default_value(0), "this is Option1")
		("Module1.Option2", po::value<std::string>()->default_value(""), "this is Option2")
		("Module2.Option3", po::value<double>()->default_value(0.0), "this is Option3");
    // clang-format on
    EXPECT_CALL(m_mockConfig, getOptionDescriptions).WillOnce(testing::Return(descriptions));
    const bool success = parser.parse();
    EXPECT_FALSE(success);
}

TEST_F(ConfigurationFileParserTest, ConfigurationFileParser_ParseInvalidJsonSyntax)
{
    std::istringstream iss(
        "{"
        "  \"Module1\": {"
        "}");
    ConfigurationFileParser parser(iss);
    parser.add(m_mockConfig);
    IConfiguration::OptionDescriptions descriptions;
    // clang-format off
    descriptions.add_options()
		("Module1.Option1", po::value<unsigned>()->default_value(0), "this is Option1")
		("Module1.Option2", po::value<std::string>()->default_value(""), "this is Option2")
		("Module2.Option3", po::value<double>()->default_value(0.0), "this is Option3");
    // clang-format on
    EXPECT_CALL(m_mockConfig, getOptionDescriptions).WillOnce(testing::Return(descriptions));
    const bool success = parser.parse();
    EXPECT_FALSE(success);
}
