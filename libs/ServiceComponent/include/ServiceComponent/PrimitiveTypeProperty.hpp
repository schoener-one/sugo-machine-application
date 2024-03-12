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

#include <string>

#include "Common/Logger.hpp"
#include "ServiceComponent/Identifier.hpp"
#include "ServiceComponent/Property.hpp"

namespace sugo::service_component
{
/// @brief Class for primitive type properties.
template <typename TypeT, TypeT Min, TypeT Max>
class PrimitiveTypeProperty : public Property<TypeT>
{
    /// Assert: minimum value must be lower or equal maximum!
    static_assert(Min <= Max);

public:
    /// @brief Creates a new property instance.
    PrimitiveTypeProperty(std::string name, TypeT value)
        : Property<TypeT>(std::move(name), std::move(value))
    {
    }

    bool setValue(const TypeT& value) override
    {
        // validate value
        if (value < Min)
        {
            LOG(error) << "Set property " << Property<TypeT>::getName() << " value " << value
                       << " is out of range " << Min;
            return false;
        }
        else if (value > Max)
        {
            LOG(error) << "Set property " << Property<TypeT>::getName() << " value " << value
                       << " is out of range " << Max;
            return false;
        }
        return Property<TypeT>::setValue(value);
    }

    /**
     * @brief Assigns a new value to the property.
     *
     * @param value Value to be assigned.
     * @return The property object.
     */
    PrimitiveTypeProperty& operator=(const TypeT& value)
    {
        (void)setValue(value);
        return *this;
    }
};
}  // namespace sugo::service_component
