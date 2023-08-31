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

#include <initializer_list>
#include <map>
#include <string>
#include <vector>

#include "Common/IConfiguration.hpp"
#include "Common/Option.hpp"

namespace sugo::common
{
/**
 * Class which contains configuration options.
 */
class Configuration : public IConfiguration
{
public:
    Configuration() = default;

    explicit Configuration(std::initializer_list<Option> list)
    {
        for (auto& option : list)
        {
            doAdd(option);
        }
    }

    ~Configuration() override = default;

    // IConfiguration {{
    OptionDescriptions getOptionDescriptions() const override
    {
        OptionDescriptions descriptions;
        for (auto& option : m_mapOptions)
        {
            descriptions.add(option.second.getDescription());
        }
        return descriptions;
    }

    bool set(const VariablesMap& mapVariables) override
    {
        for (auto& item : mapVariables)
        {
            auto option = m_mapOptions.find(item.first);
            if (option != m_mapOptions.end())
            {
                option->second.setValue(item.second.value());
            }
        }
        return true;
    }

    void add(const Option& option) override
    {
        doAdd(option);
    }

    const Option& getOption(const std::string& name) const override
    {
        const auto it = m_mapOptions.find(name);
        assert(it != m_mapOptions.end());
        return it->second;
    }

    const IConfiguration& extract(const std::string& prefix,
                                  IConfiguration&    extractedConfiguration) const override;
    // IConfiguration }}

private:
    void doAdd(const Option& option)
    {
        m_mapOptions.insert({option.getName(), option});
    }

    /// List of options
    using OptionMap = std::map<std::string, Option>;
    OptionMap m_mapOptions;
};

}  // namespace sugo::common
