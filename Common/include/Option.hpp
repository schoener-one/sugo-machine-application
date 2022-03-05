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
#include <vector>

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
    using ValueSemantic = boost::program_options::value_semantic;

    Option()
    {
    }

    Option(const Option& option) : m_description(option.m_description), m_value(option.m_value)
    {
        applyDefault();
    }

    template <class ValueT>
    Option(const std::string& name, ValueT value, const std::string& description,
           bool isMultitoken = false)
        : m_description(boost::make_shared<boost::program_options::option_description>(
              name.c_str(),
              (isMultitoken
                   ? boost::program_options::value<ValueT>()->multitoken()->default_value(value)
                   : boost::program_options::value<ValueT>()->default_value(value)),
              description.c_str()))
    {
        applyDefault();
    }

    Option(const std::string& name, const ValueSemantic* semantic, const std::string& description)
        : m_description(boost::make_shared<boost::program_options::option_description>(
              name.c_str(), semantic, description.c_str()))
    {
        applyDefault();
    }

    const std::string& getName() const
    {
        return m_description->long_name();
    }

    const boost::program_options::option_description& getDescription()
    {
        assert(m_description);
        return *m_description.get();
    }

    void applyDefault()
    {
        m_description->semantic()->apply_default(m_value);
    }

    template <class ValueT>
    const ValueT get() const
    {
        return boost::any_cast<ValueT>(m_value);
    }

    Option& operator=(const Option& option)
    {
        m_description = option.m_description;
        m_value       = option.m_value;
        return *this;
    }

    friend Configuration;

private:
    boost::shared_ptr<boost::program_options::option_description> m_description;
    boost::any                                                    m_value;
};

}  // namespace sugo

#endif  // OPTION_HPP_
