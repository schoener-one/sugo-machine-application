///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   04.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

#include "ConfigurationParser.hpp"

#include "Logger.hpp"

using namespace sugo;
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
