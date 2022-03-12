///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-07
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "IConfiguration.hpp"

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
    /// Hal identifier type
    using Identifier = std::string;

    /// Hal object map
    template <class HalObjectT>
    using Map = std::map<Identifier, std::unique_ptr<HalObjectT>>;

    /**
     * @brief Construct a new IHalObject object
     *
     * @param id Identifier of that object
     */
    IHalObject(const Identifier& id) : m_id(id)
    {
    }

    /**
     * @brief Destroy the IHalObject object.
     *
     */
    virtual ~IHalObject()
    {
    }

    /**
     * @brief Initializes the hardware abstraction layer object
     *
     * @param configuration Configuration to be applied
     * @return true  If could be initialized successfully
     * @return false If could not be initialized successfully
     */
    virtual bool init(const IConfiguration& configuration) = 0;

    /**
     * @brief Get the Id object
     *
     * @return Identifier&
     */
    virtual const Identifier& getId() const
    {
        return m_id;
    }

private:
    Identifier m_id;
};

}  // namespace sugo::hal
