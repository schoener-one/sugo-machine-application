///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   03.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef ICONFIGURATION_HPP_
#define ICONFIGURATION_HPP_

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include "Option.hpp"

namespace sugo
{
/**
 * Interface class for configurations
 */
class IConfiguration
{
protected:
    IConfiguration() = default;

public:
    virtual ~IConfiguration() = default;

    using OptionDescriptions = boost::program_options::options_description;
    using VariablesMap       = boost::program_options::variables_map;
    using OptionList         = std::vector<Option>;

    virtual OptionDescriptions getOptionDescriptions() const            = 0;
    virtual bool               set(const VariablesMap& vm)              = 0;
    virtual void               add(const Option& option)                = 0;
    virtual const Option&      getOption(const std::string& name) const = 0;

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

    const Option& operator[](const std::string& name) const
    {
        return getOption(name);
    }
};

}  // namespace sugo

#endif  // ICONFIGURATION_HPP_
