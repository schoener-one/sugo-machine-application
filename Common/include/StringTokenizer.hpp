///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   20.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace sugo
{
/**
 * @brief Class to split a string into tokens.
 */
struct StringTokenizer
{
    /**
     * @brief Token to be used for splitting
     *
     * @tparam delimiter Delimiter to be used for splitting.
     */
    template <char delimiter = ' '>
    struct Token : std::string
    {
    };
    /// String tokens
    using Tokens = std::vector<std::string>;

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
 splitting.

 */
template <char delimiter>
std::istream& operator>>(std::istream& in, StringTokenizer::Token<delimiter>& out)
{
    std::getline(in, out, delimiter);
    return in;
}

}  // namespace sugo
