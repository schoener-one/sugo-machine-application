///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   04.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef CONFIGURATIONFILEPARSER_HPP_
#define CONFIGURATIONFILEPARSER_HPP_

#include <istream>

#include "ConfigurationParser.hpp"

namespace sugo
{
/**
 * Class to parse configuration files. The supported configuration file format
 * is JSON.
 */
class ConfigurationFileParser : public ConfigurationParser
{
public:
    /**
     * @param inStream Input stream to be parsed. The stream must be valid
     * until parsing has been don.
     */
    explicit ConfigurationFileParser(std::istream& inStream) : m_inStream(inStream)
    {
    }

protected:
    // ConfigurationParser {{
    ParsedOptions runParser(const IConfiguration::OptionDescriptions& optionDescriptions) override;
    // ConfigurationParser }}

private:
    std::istream& m_inStream;
};

}  // namespace sugo

#endif  // CONFIGURATIONFILEPARSER_HPP_
