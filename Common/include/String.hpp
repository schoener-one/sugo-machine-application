///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   20.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef STRING_HPP_
#define STRING_HPP_

#include <sstream>
#include <string>
#include <vector>

namespace moco
{
template <char delimiter>
class Token : public std::string
{
};

template <char delemiter>
std::istream& operator>>(std::istream& in, Token<delemiter>& out)
{
    std::getline(in, out, delemiter);
    return in;
}

/**
 * Helper class for string handling.
 */
class String
{
public:
    /// String tokens
    using Tokens = std::vector<std::string>;

    /**
     * Splits a string into tokens.
     * @tparam delimiter Delimiter for splitting the input string.
     * @param in Input string.
     * @return Tokens splitted from the string.
     */
    template <char delimiter>
    static Tokens split(const std::string& in)
    {
        std::istringstream iss(in);
        return std::vector<std::string>((std::istream_iterator<Token<delimiter>>(iss)),
                                        std::istream_iterator<Token<delimiter>>());
    }
};
}  // namespace moco

#endif  // STRING_HPP_
