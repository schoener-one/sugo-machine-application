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

#include <functional>

#include "Common/Types.hpp"

namespace sugo::service_component
{
//#// @brief
/**
 * @brief Interface class for properties.
 *
 * @tparam TypeTT Property value type.
 */
template <typename TypeT>
class IProperty
{
public:
    /// @brief Property value type.
    using Type = TypeT;

    /// @brief Default destructor
    virtual ~IProperty() = default;

    /**
     * @brief Returns the name of the property.
     *
     * @return Name of the property.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Handler definition which handles a notification message.
     *
     * @param changedProperty The changed property.
     */
    using ValueChangeHandler = std::function<void(const IProperty<TypeT>& changedProperty)>;

    /**
     * @brief Sets the validated property value.
     * A value validation could lead to a value clipping in case the new value is out of valid
     * range!
     *
     * @param value
     * @return true If the value could be set.
     * @return false If the value could not be set.
     */
    virtual bool setValue(const TypeT& value) = 0;

    /**
     * @brief Returns the property value.
     * 
     * @return Returns the property value.
     */
    virtual const TypeT& getValue() const = 0;

    /**
     * @brief Returns the property value as JSON object.
     *
     * @return JSON object which includes the property value.
     */
    virtual common::Json getValueAsJson() const = 0;

    /**
     * @brief Sets the property value from JSON object.
     *
     * @param value JSON formatted string which contains the property value.
     * @return true If the value could be set.
     * @return false If the value could not be set.
     */
    virtual bool setValueFromJson(const std::string& value) = 0;

    /**
     * @brief Registers a value change handler for this property.
     * The handler will be called every time a new property value is set.
     *
     * @param handler Value change handler to be registered.
     */
    virtual void registerValueChangeHandler(ValueChangeHandler handler) = 0;
};
}  // namespace sugo::service_component
