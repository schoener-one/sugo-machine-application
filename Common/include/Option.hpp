///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   15.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef OPTION_HPP_
#define OPTION_HPP_

#include <cassert>
#include <string>

#include <boost/any.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/shared_ptr.hpp>

namespace sugo
{
class Configuration;

/**
 * Class which contains a single option.
 */
class Option
{
public:
    using OptionDescription = boost::program_options::option_description;
    using Description       = boost::shared_ptr<OptionDescription>;

    Option()
    {
    }

    Option(const Option& option)
        : m_description(option.m_description), m_value(option.m_value), m_name(option.m_name)
    {
    }

    Option(const std::string newName, const Option& option)
        : m_description(option.m_description), m_value(option.m_value), m_name(newName)
    {
    }

    template <class ValueT>
    Option(const std::string& name, ValueT value, const std::string& description,
           bool isMultitoken = false)
        : m_description(boost::make_shared<OptionDescription>(
              name.c_str(),
              (isMultitoken
                   ? boost::program_options::value<ValueT>()->multitoken()->default_value(value)
                   : boost::program_options::value<ValueT>()->default_value(value)),
              description.c_str())),
          m_name(m_description->long_name())
    {
        applyDefault();
    }

    const std::string& getName() const
    {
        return m_name;
    }

    const Description& getDescription() const
    {
        assert(m_description);
        return m_description;
    }

    template <class ValueT>
    const ValueT get() const
    {
        return boost::any_cast<ValueT>(m_value);
    }

    void setValue(boost::any newValue)
    {
        m_value = newValue;
    }

    Option& operator=(const Option&) = default;

private:
    void applyDefault()
    {
        m_description->semantic()->apply_default(m_value);
    }

    Description m_description;
    boost::any  m_value;
    std::string m_name;
};

}  // namespace sugo

#endif  // OPTION_HPP_
