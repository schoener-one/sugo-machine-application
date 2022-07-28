///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.12.2019
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IConfiguration.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{
/**
 * @brief Class representing the machine application.
 */
class ComponentsExecutionGroup final
{
public:
    ~ComponentsExecutionGroup() = default;

    /**
     * @brief Adds all configuration options of all components.
     *
     * @param configuration Configuration object to which all options are added.
     */
    void addConfigurationOptions(IConfiguration& configuration);

    /**
     * @brief Executes all components in a sufficient sequence.
     * This call does not return until stop() has been called or a an fatal
     * error has been occurred!
     *
     * @param  serviceLocator Service locator object
     * @return true if all components could be executed successfully and have been stopped again.
     * @return false if at least one component failed to start.
     */
    bool start(const ServiceLocator& serviceLocator);
};

}  // namespace sugo
