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

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

#include "Common/ConfigurationParser.hpp"
#include "Common/Logger.hpp"

using namespace sugo::common;
namespace po = boost::program_options;

bool ConfigurationParser::parse()
{
    bool                                  success = true;
    boost::program_options::variables_map variablesMap;
    po::options_description               commandLineOpts = getOptionDescriptions();

    try
    {
        for (auto configuration : m_configurationList)
        {
            const po::options_description descriptions = configuration->getOptionDescriptions();
            commandLineOpts.add(descriptions);
        }

        po::store(parseOptionDescriptions(commandLineOpts), variablesMap);
        po::notify(variablesMap);
    }
    catch (std::exception& ex)
    {
        LOG(error) << "Exception: " << ex.what();
        success = false;
    }
    notifyFinished(variablesMap, commandLineOpts, success);

    return success;
}

bool ConfigurationParser::notifyFinished(const VariablesMap& variablesMap,
                                         const IConfiguration::OptionDescriptions&,
                                         bool withSuccess)
{
    if (withSuccess)
    {
        for (auto configuration : m_configurationList)
        {
            withSuccess = configuration->set(variablesMap);
            if (!withSuccess)
            {
                break;
            }
        }
    }
    return withSuccess;
}
