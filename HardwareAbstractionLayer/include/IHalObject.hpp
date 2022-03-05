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

namespace sugo::hal
{
/**
 * @brief Base class of all HAL objects
 *
 */
class IHalObject
{
public:
    using Identifier = std::string;  ///< Used to identify hardware components

    /**
     * @brief Destroy the IHalObject object.
     *
     */
    virtual ~IHalObject() {}

    /**
     * @brief Get the Id object
     *
     * @return Identifier&
     */
    virtual const Identifier& getId() const = 0;

protected:
    /**
     * @brief Construct a new IHalObject object
     *
     */
    IHalObject() {}
};

}  // namespace sugo::hal
