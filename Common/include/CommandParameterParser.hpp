/*
 * CommandParameterParser.hpp
 *
 *  Created on: 29.02.2020
 *      Author: denis
 */

#ifndef INCLUDE_COMMANDPARAMETERPARSER_HPP_
#define INCLUDE_COMMANDPARAMETERPARSER_HPP_

#include <exception>
#include <string>

#include "Globals.hpp"

namespace sugo
{
/**
 * Helper class for parsing JSON parameters.
 */
class CommandParameterParser
{
public:
    explicit CommandParameterParser(const std::string& parameters) : m_parameters(parameters)
    {
    }

    class CommandParameterParseException : public std::exception
    {
    public:
        explicit CommandParameterParseException(const std::string& what) : m_message(what)
        {
        }

        const char* what() const noexcept override
        {
            return m_message.c_str();
        }

    private:
        const std::string m_message;
    };

    template <class ValueT>
    ValueT get(const std::string& parameterName) const
    {
        auto params = Json::parse(m_parameters);
        auto value  = params.at(parameterName);
        if (value.empty())
        {
            throw CommandParameterParseException("mandatory parameter " + parameterName +
                                                 " not found");
        }

        return params[parameterName].get<ValueT>();
    }

private:
    const std::string& m_parameters;
};

}  // namespace sugo

#endif /* INCLUDE_COMMANDPARAMETERPARSER_HPP_ */
