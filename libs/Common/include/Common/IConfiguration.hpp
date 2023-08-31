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

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <string>

#include "Common/Option.hpp"

namespace sugo::common
{
/**
 * @brief Interface class which represents a program configuration.
 * The configuration handling is based on boost::program_options!
 */
class IConfiguration
{
public:
    virtual ~IConfiguration() = default;

    using OptionDescriptions = boost::program_options::options_description;
    using VariablesMap       = boost::program_options::variables_map;
    using OptionList         = std::vector<Option>;

    /**
     * @brief Returns the option descriptions
     *
     * @return List of option descriptions.
     */
    virtual OptionDescriptions getOptionDescriptions() const = 0;

    /**
     * @brief Sets a new variable map.
     *
     * @param vm Variable map to be set.
     * @return true If the variable could be set.
     * @return false If the variable could not be set.
     */
    virtual bool set(const VariablesMap& vm) = 0;

    /**
     * @brief Adds a new options to the configuration.
     *
     * @param option Option to be added.
     */
    virtual void add(const Option& option) = 0;

    /**
     * @brief Returns the option according to the name.
     *
     * @param name Name of the option to be returned. The name must exist.
     * @return The option object according to the name.
     */
    virtual const Option& getOption(const std::string& name) const = 0;

    /**
     * Extracts all options with the passed prefix and returns a temporary configuration
     * object which contains the subset of options according to the prefix.
     * The prefix will be stripped from the new added options names.
     * @param prefix Prefix to used for extraction.
     * @param extractedConfiguration Extracted configuration object.
     * @return Extracted configuration object.
     */
    virtual const IConfiguration& extract(const std::string& prefix,
                                          IConfiguration&    extractedConfiguration) const = 0;

    /**
     * @brief Bracket operator overload
     *
     * @param name Name of the option to be returned. Caller has to take care that the
     * option exists!
     * @return Selected option.
     */
    const Option& operator[](const std::string& name) const
    {
        return getOption(name);
    }
};
}  // namespace sugo::common
