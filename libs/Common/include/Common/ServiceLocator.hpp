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
#include <cassert>
#include <unordered_map>

namespace sugo::common
{
/// @brief Class implemention of the service locator pattern to provide global service objects.
class ServiceLocator final
{
public:
    /**
     * @brief Adds a new service object to the container.
     *
     * @tparam ServiceT Type of the service object.
     * @param service The service object to be added.
     */
    template <class ServiceT>
    void add(ServiceT& service)
    {
        const size_t hash = typeid(ServiceT).hash_code();
        m_services.emplace(hash, boost::any(&service));
    }

    /**
     * @brief Returns the previously added object.
     *
     * @tparam ServiceT Type of the service object.
     * @return The service object.
     */
    template <class ServiceT>
    ServiceT& get() const
    {
        const size_t hash = typeid(ServiceT).hash_code();
        assert(m_services.count(hash) == 1);
        ServiceT* service = boost::any_cast<ServiceT*>(m_services.at(hash));
        assert(service != nullptr);
        return *service;
    }

private:
    std::unordered_map<size_t, boost::any> m_services;  ///< Map of service objects.
};

}  // namespace sugo::common