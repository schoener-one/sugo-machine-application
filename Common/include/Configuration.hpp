///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <initializer_list>
#include <map>
#include <string>
#include <vector>

#include "IConfiguration.hpp"
#include "Option.hpp"

namespace sugo
{
/**
 * Class which contains configuration options.
 */
class Configuration : public IConfiguration
{
public:
    Configuration()
    {
    }

    explicit Configuration(std::initializer_list<Option> list)
    {
        for (auto& option : list)
        {
            add(option);
        }
    }

    virtual ~Configuration()
    {
    }

    // IConfiguration {{
    virtual OptionDescriptions getOptionDescriptions() const override
    {
        OptionDescriptions descriptions;
        for (auto& option : m_mapOptions)
        {
            descriptions.add(option.second.m_description);
        }
        return descriptions;
    }

    virtual bool set(const VariablesMap& mapVariables) override
    {
        for (auto& item : mapVariables)
        {
            auto option = m_mapOptions.find(item.first);
            if (option != m_mapOptions.end())
            {
                option->second.m_value = item.second.value();
            }
        }
        return true;
    }

    void add(const Option& option) override
    {
        m_mapOptions.insert({option.getName(), option});
    }

    const Option& getOption(const std::string& name) const override
    {
        return m_mapOptions.at(name);
    }

    const IConfiguration& extract(const std::string& prefix,
                                  IConfiguration&    extractedConfiguration) const override;
    // IConfiguration }}

private:
    /// List of options
    using OptionMap = std::map<std::string, Option>;
    OptionMap m_mapOptions;
};

}  // namespace sugo

#endif  // CONFIGURATION_HPP_
