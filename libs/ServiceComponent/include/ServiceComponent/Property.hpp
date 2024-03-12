///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   15.03.2024
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

#include "ServiceComponent/IProperty.hpp"

namespace sugo::service_component
{
/// @brief Base class for properties.
template <typename TypeT>
class Property : public IProperty<TypeT>
{
public:
    using typename IProperty<TypeT>::ValueChangeHandler;

    /// @brief Creates a new property object
    Property(std::string name, const TypeT& value)
        : m_name(std::move(name)), m_value(std::move(value))
    {
    }

    std::string getName() const override
    {
        return m_name;
    }

    const TypeT& getValue() const override
    {
        return m_value;
    }

    bool setValue(const TypeT& value) override
    {
        if (value != m_value)
        {
            m_value = value;

            if (m_changeHandler)
            {
                m_changeHandler(*this);
            }
        }
        return true;
    }

    /**
     * @brief Property value operator.
     *
     * @return Returns the property value.
     */
    operator const TypeT&() const
    {
        return getValue();
    }

    common::Json getValueAsJson() const override
    {
        return common::Json({{id::PropertyValue, m_value}});
    }

    bool setValueFromJson(const std::string& jsonValue) override
    {
        // FIXME do not allow throwing exception during parsing!
        auto        json  = common::Json::parse(jsonValue);
        const auto& value = json.at(id::PropertyValue);

        if (value.empty())
        {
            LOG(error) << "Failed to set property " << getName()
                       << " from JSON because of improperly formed JSON payload";
            return false;
        }

        // FIXME could throw an exception!
        return setValue(value.get<TypeT>());
    }

    void registerValueChangeHandler(ValueChangeHandler handler) override
    {
        m_changeHandler = handler;
    }

private:
    const std::string  m_name;             ///< Property name.
    TypeT              m_value{};          ///< Property value.
    ValueChangeHandler m_changeHandler{};  ///< Registered value change handler.
};
}  // namespace sugo::service_component
