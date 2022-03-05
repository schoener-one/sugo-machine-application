///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IHalObject.hpp"

namespace sugo::hal
{
/**
 * @brief Base class of all HAL objects
 *
 */
class HalObject : virtual public IHalObject
{
public:
    /**
     * @brief Construct a new HalObject object
     *
     */
    HalObject(const Identifier& id) : m_id(id) {}

    /**
     * @brief Destroy the HalObject object.
     *
     */
    virtual ~HalObject() {}

    const Identifier& getId() const override { return m_id; }

private:
    const Identifier m_id;
};

}  // namespace sugo::hal
