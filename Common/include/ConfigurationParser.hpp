///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   04.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef CONFIGURATIONPARSER_HPP_
#define CONFIGURATIONPARSER_HPP_

#include <list>

#include <boost/program_options/parsers.hpp>
#include "IConfiguration.hpp"

namespace sugo
{
class ConfigurationParser
{
public:
    virtual ~ConfigurationParser() {}

    /** Adds a new configuration to the parser.
     *
     * @param configuration Configuration set to be added.
     */
    void add(IConfiguration& configuration) { m_configurationList.push_back(&configuration); }

    /**
     * Parses the command line option passed based on the set configurations.
     *
     * @param helpOutStream Pointer to a stream which receives the help message.
     *
     * @return true if the arguments could be parsed successfully.
     */
    bool parse();

protected:
    ConfigurationParser() {}

    using ParsedOptions = boost::program_options::parsed_options;
    using VariablesMap  = boost::program_options::variables_map;

    virtual IConfiguration::OptionDescriptions getOptionDescriptions()
    {
        return std::move(IConfiguration::OptionDescriptions());
    }

    virtual ParsedOptions runParser(
        const IConfiguration::OptionDescriptions& optionDescriptions) = 0;

    virtual bool notifyFinished(const VariablesMap&                       variablesMap,
                                const IConfiguration::OptionDescriptions& optionDescriptions,
                                bool                                      withSuccess);

    using ConfigurationList = std::list<IConfiguration*>;
    ConfigurationList m_configurationList;
};

}  // namespace sugo

#endif  // CONFIGURATIONPARSER_HPP_
