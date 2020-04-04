///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   03.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMON_INCLUDE_ICONFIGURATION_HPP_
#define COMMON_INCLUDE_ICONFIGURATION_HPP_

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include "Option.hpp"

namespace moco
{
/**
 * Interface class for configurations
 */
class IConfiguration
{
protected:
    IConfiguration() {}

public:
    virtual ~IConfiguration() {}

    using OptionDescriptions = boost::program_options::options_description;
    using VariablesMap       = boost::program_options::variables_map;

    virtual OptionDescriptions getOptionDescriptions() const            = 0;
    virtual bool               set(const VariablesMap& vm)              = 0;
    virtual void               add(const Option& option)                = 0;
    virtual const Option&      getOption(const std::string& name) const = 0;
};

}  // namespace moco

#endif  // COMMON_INCLUDE_ICONFIGURATION_HPP_
