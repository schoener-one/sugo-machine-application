///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "CommandLineParser.hpp"
#include "IConfigurationMock.hpp"

namespace po = boost::program_options;
using namespace moco;

class CommandLineParserTest : public ::testing::Test
{
protected:
    CommandLineParserTest() {}

    ~CommandLineParserTest() override {}

    static void SetUpTestCase() {}

    void SetUp() override {}

    void TearDown() override {}

    IConfigurationMock m_mockConfig;
};

TEST_F(CommandLineParserTest, CommandLineParser_Parse)
{
    char const*                        args[] = {"-f", "42", "--bar", "42"};
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
