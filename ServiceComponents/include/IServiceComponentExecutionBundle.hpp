///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2019-12-01
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRunnable.hpp"
#include "IServiceComponent.hpp"

#include <string>

namespace sugo
{
/**
 * @brief Interface class for execution bundles.
 *
 */
class IServiceComponentExecutionBundle : public IRunnable
{
public:
    virtual ~IServiceComponentExecutionBundle() = default;

    /**
     * @brief Returns the id of the component.
     *
     * @return Component id
     */
    virtual const std::string& getId() const = 0;

    /**
     * @brief Waits until the component main process execution has finished.
     *
     */
    virtual void waitUntilFinished() = 0;

    /**
     * @brief Get the Service Component object
     * 
     * @return ServiceComponent& 
     */
    virtual IServiceComponent& getServiceComponent() = 0;
};

}  // namespace sugo