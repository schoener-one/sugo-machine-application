///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
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

#include "Common/IConfiguration.hpp"
#include "Identifier.hpp"

namespace sugo::hal
{
/**
 * @brief Base class of all HAL objects
 *
 * @todo Split interface and functionality into two classes (e.g. IHalObject<--HalObject)
 */
class IHalObject
{
public:
    /// Hal object map
    template <class HalObjectT>
    using Map = std::map<Identifier, std::shared_ptr<HalObjectT>>;

    /**
     * @brief Construct a new IHalObject object
     *
     * @param id Identifier of that object
     */
    explicit IHalObject(Identifier id) : m_id(std::move(id))
    {
    }

    /**
     * @brief Destroys the IHalObject object.
     *
     */
    virtual ~IHalObject() = default;

    /**
     * @brief Initializes the hardware abstraction layer object
     *
     * @param configuration common::Configuration to be applied
     * @return true  If could be initialized successfully
     * @return false If could not be initialized successfully
     */
    virtual bool init(const common::IConfiguration& configuration) = 0;

    /**
     * @brief Get the Id object
     *
     * @return Identifier&
     */
    virtual const Identifier& getId() const
    {
        return m_id;
    }

    /**
     * @brief Compares the a HAL object own id with a another id.
     *
     * @param id     Id to be compared.
     * @return true  If the object id is equal to the compared id.
     * @return false If the object id is not equal to the compared id.
     */
    bool operator==(const hal::Identifier& id) const
    {
        return (id == getId());
    }

private:
    const Identifier m_id;
};

}  // namespace sugo::hal
