///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   04.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <fstream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ConfigurationFileParser.hpp"

using namespace sugo;
namespace pt = boost::property_tree;
namespace po = boost::program_options;

namespace
{
template <class charT>
class JsonParser
{
public:
    explicit JsonParser(std::basic_istream<charT>& inStream) : m_inStream(inStream)
    {
    }

    static void pushOption(const std::basic_string<charT>& name,
                           const std::basic_string<charT>& value, po::parsed_options& options)
    {
        // unregistered not allowed!
        if (options.description->find_nothrow(name, false) == nullptr)
        {
            boost::throw_exception(po::unknown_option(name));
        }

        if (!options.options.empty() && options.options.back().string_key == name)
        {
            auto& option = options.options.back();
            assert(!option.value.empty());
            option.value.push_back(value);
        }
        else
        {
            po::basic_option<charT> opt;
            opt.string_key = name;
            opt.value.push_back(value);
            opt.unregistered     = false;
            opt.position_key     = -1;
            opt.case_insensitive = true;
            options.options.push_back(opt);
        }
    }

    static void parseChildren(std::string prefix, pt::ptree& tree, po::parsed_options& options)
    {
        if (tree.size() == 0)
        {
            pushOption(prefix.substr(1), tree.data(), options);
        }
        else
        {
            for (auto it = tree.begin(); it != tree.end(); ++it)
            {
                if (it->first.empty())
                {
                    pushOption(prefix.substr(1), it->second.data(), options);
                }
                else
                {
                    parseChildren(prefix + "." + it->first, it->second, options);
                }
            }
        }
    }

    void parseJsonOptions(po::parsed_options& options)
    {
        pt::basic_ptree<std::basic_string<charT>, std::basic_string<charT>> pt;
        pt::read_json(m_inStream, pt);
        parseChildren(std::basic_string<charT>(), pt, options);
    }

    po::basic_parsed_options<charT> parse(const po::options_description& desc)
    {
        po::parsed_options result(&desc);
        parseJsonOptions(result);
        return po::basic_parsed_options<charT>(result);
    }

    std::basic_istream<charT>& m_inStream;
};
}  // namespace

ConfigurationParser::ParsedOptions ConfigurationFileParser::runParser(
    const IConfiguration::OptionDescriptions& optionDescriptions)
{
    ::JsonParser<char> parser(m_inStream);
    return parser.parse(optionDescriptions);
}
