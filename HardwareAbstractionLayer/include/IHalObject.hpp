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
/// @brief HAL object identifier type
using Identifier = std::string;

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
    explicit IHalObject(const Identifier& id) : m_id(id)
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
