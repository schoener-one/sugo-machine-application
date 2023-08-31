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

#include <boost/program_options/parsers.hpp>
#include <list>

#include "Common/IConfiguration.hpp"

namespace sugo::common
{
class ConfigurationParser
{
public:
    virtual ~ConfigurationParser() = default;

    /** Adds a new configuration to the parser.
     *
     * @param configuration Configuration set to be added.
     */
    void add(IConfiguration& configuration)
    {
        m_configurationList.push_back(&configuration);
    }

    /**
     * Parses the command line option passed based on the set configurations.
     *
     * @param helpOutStream Pointer to a stream which receives the help message.
     *
     * @return true if the arguments could be parsed successfully.
     */
    bool parse();

protected:
    using ParsedOptions = boost::program_options::parsed_options;
    using VariablesMap  = boost::program_options::variables_map;

    virtual IConfiguration::OptionDescriptions getOptionDescriptions()
    {
        return std::move(IConfiguration::OptionDescriptions());
    }

    virtual ParsedOptions parseOptionDescriptions(
        const IConfiguration::OptionDescriptions& optionDescriptions) = 0;

    virtual bool notifyFinished(const VariablesMap&                       variablesMap,
                                const IConfiguration::OptionDescriptions& optionDescriptions,
                                bool                                      withSuccess);

private:
    using ConfigurationList = std::list<IConfiguration*>;
    ConfigurationList m_configurationList;
};

}  // namespace sugo::common
