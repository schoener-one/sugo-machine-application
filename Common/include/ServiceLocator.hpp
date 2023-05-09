///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   28.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/any.hpp>
#include <cassert>
#include <unordered_map>

namespace sugo
{
/**
 * @brief Class used to keep global service object.
 *
 */
class ServiceLocator final
{
public:
    template <class ServiceT>
    void add(ServiceT& service)
    {
        const size_t hash = typeid(ServiceT).hash_code();
        m_services.emplace(hash, boost::any(&service));
    }

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
    std::unordered_map<size_t, boost::any> m_services;
};

}  // namespace sugo