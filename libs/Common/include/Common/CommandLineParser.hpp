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

#pragma once

#include <iostream>

#include "Common/ConfigurationParser.hpp"

namespace sugo::common
{
/// @brief Class to parse command line arguments.
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
    IConfiguration::OptionDescriptions getOptionDescriptions() override
    {
        IConfiguration::OptionDescriptions optionDescriptions("Allowed commandline options");
        optionDescriptions.add_options()("help", "this help message");
        return optionDescriptions;
    }

    ParsedOptions parseOptionDescriptions(
        const IConfiguration::OptionDescriptions& optionDescriptions) override
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

private:
    int          m_argumentCount;
    char const** m_arguments;
};

}  // namespace sugo::common
