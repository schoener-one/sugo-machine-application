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

#include <istream>

#include "Common/ConfigurationParser.hpp"

namespace sugo::common
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
    ParsedOptions parseOptionDescriptions(
        const IConfiguration::OptionDescriptions& optionDescriptions) override;

private:
    std::istream& m_inStream;
};

}  // namespace sugo::common
