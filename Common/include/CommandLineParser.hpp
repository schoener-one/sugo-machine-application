///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMANDLINEPARSER_HPP_
#define COMMANDLINEPARSER_HPP_

#include <iostream>

#include "ConfigurationParser.hpp"

namespace sugo
{
/**
 * Class to parse command line arguments.
 */
class CommandLineParser : public ConfigurationParser
{
public:
    /**
     * @param argc          Argument count.
     * @param argv          Argument string vector.
     */
    CommandLineParser(int argc, char const** argv) : m_argumentCount(argc), m_arguments(argv)
    {
    }

protected:
    // ConfigurationParser {{
    IConfiguration::OptionDescriptions getOptionDescriptions() override
    {
        IConfiguration::OptionDescriptions optionDescriptions("Allowed commandline options");
        optionDescriptions.add_options()("help", "this help message");
        return optionDescriptions;
    }

    ParsedOptions runParser(const IConfiguration::OptionDescriptions& optionDescriptions) override
    {
        return boost::program_options::parse_command_line(m_argumentCount, m_arguments,
                                                          optionDescriptions);
    }

    bool notifyFinished(const VariablesMap&                       variablesMap,
                        const IConfiguration::OptionDescriptions& optionDescriptions,
                        bool                                      withSuccess) override
    {
        if (!withSuccess && variablesMap.count("help"))
        {
            std::cerr << optionDescriptions << std::endl;
            withSuccess = false;
        }
        return ConfigurationParser::notifyFinished(variablesMap, optionDescriptions, withSuccess);
    }
    // ConfigurationParser }}

private:
    int          m_argumentCount;
    char const** m_arguments;
};

}  // namespace sugo

#endif  // COMMANDLINEPARSER_HPP_
