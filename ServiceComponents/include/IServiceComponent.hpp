///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   09.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef ISERVICECOMPONENT_HPP_
#define ISERVICECOMPONENT_HPP_

#include "IRunnable.hpp"

namespace sugo
{
/**
 * Interface class of all service components.
 */
class IServiceComponent : public IRunnable
{
protected:
    IServiceComponent() {}

public:
    virtual ~IServiceComponent() {}
};
}  // namespace sugo

#endif  // ISERVICECOMPONENT_HPP_
