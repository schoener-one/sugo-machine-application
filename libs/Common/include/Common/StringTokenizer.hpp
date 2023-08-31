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

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace sugo::common
{
/// @brief Class to split a string into tokens.
class StringTokenizer
{
public:
    /// String tokens
    using Tokens = std::vector<std::string>;

    /**
     * @brief Token to be used for splitting
     *
     * @tparam delimiter Delimiter to be used for splitting.
     */
    template <char delimiter = ' '>
    struct Token : std::string
    {
    };

    /**
     * @brief Splits a string into tokens.
     *
     * @tparam delimiter Delimiter to be used for splitting.
     * @param in         Input string to be splitted.
     * @return           List of splitted tokens.
     */
    template <char delimiter = ' '>
    static Tokens split(const std::string& in)
    {
        std::istringstream iss(in);
        return std::vector<std::string>((std::istream_iterator<Token<delimiter>>(iss)),
                                        std::istream_iterator<Token<delimiter>>());
    }
};

/**
 * @brief Helper assignment operator to parse an input stream into a token
 *
 * @tparam delimiter Delimiter to be used for splitting.
 * @param in         Input stream to read from.
 * @param out        output token object.
 * @return           resulting stream object. * @tparam delimiter Delimiter to be used for
 * splitting.
 */
template <char delimiter>
std::istream& operator>>(std::istream& in, StringTokenizer::Token<delimiter>& out)
{
    std::getline(in, out, delimiter);
    return in;
}

}  // namespace sugo::common
