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

#include <boost/any.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <string>

namespace sugo::common
{
/// @brief Class which contains a single option.
class Option
{
public:
    using OptionDescription = boost::program_options::option_description;
    using Description       = boost::shared_ptr<OptionDescription>;

    Option()                     = default;
    Option(const Option& option) = default;
    Option(Option&& option)      = default;

    Option(std::string newName, const Option& option)
        : m_description(option.m_description), m_value(option.m_value), m_name(std::move(newName))
    {
    }

    template <class ValueT>
    Option(const std::string& name, ValueT value, const std::string& description,
           bool isMultitoken = false)
        // TODO remove dynamic memory allocation!
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
        assert((m_description != nullptr));
        return m_description;
    }

    template <class ValueT>
    const ValueT get() const
    {
        return boost::any_cast<ValueT>(m_value);
    }

    void setValue(const boost::any& newValue)
    {
        m_value = newValue;
    }

    Option& operator=(const Option&) = default;
    Option& operator=(Option&&) = default;

private:
    void applyDefault()
    {
        m_description->semantic()->apply_default(m_value);
    }

    Description m_description;
    boost::any  m_value;
    std::string m_name;
};

}  // namespace sugo::common
