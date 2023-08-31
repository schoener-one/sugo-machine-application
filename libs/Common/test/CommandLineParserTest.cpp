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

#include "Common/CommandLineParser.hpp"
#include "Common/IConfigurationMock.hpp"

namespace po = boost::program_options;
using namespace sugo::common;

class CommandLineParserTest : public ::testing::Test
{
protected:
    IConfigurationMock m_mockConfig;
};

TEST_F(CommandLineParserTest, CommandLineParser_Parse)
{
    char const*       args[] = {"-f", "42", "--bar", "42"};
    CommandLineParser parser(sizeof(args) / sizeof(args[0]), args);
    parser.add(m_mockConfig);
    IConfiguration::OptionDescriptions descriptions;
    // clang-format off
    descriptions.add_options()
    	("foo,f", po::value<unsigned>()->default_value(41), "this is foo")
    	("bar,b", po::value<unsigned>()->default_value(41), "this is bar");
    // clang-format on
    EXPECT_CALL(m_mockConfig, getOptionDescriptions).WillOnce(testing::Return(descriptions));
    EXPECT_CALL(m_mockConfig, set).WillOnce(testing::Return(true));
    const bool success = parser.parse();
    EXPECT_TRUE(success);
}
