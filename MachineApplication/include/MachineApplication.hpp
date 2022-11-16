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

#include <string>

#include "Configuration.hpp"
#include "ServiceComponentsExecutionGroup.hpp"

namespace sugo
{
/**
 * @brief Abstreact class representing the machine application.
 */
class MachineApplication final
{
public:
    /**
     * @brief Construct a new Machine Application object
     *
     * @param appName Name of the application
     */
    explicit MachineApplication(std::string appName);

    /**
     * Starts the application with command line arguments.
     * @param argc Argument count.
     * @param argv Pointer to a vector of string passed as arguments.
     * @return     0 if the application run successfully, value smaller
     *             than 0 to indicate an error.
     */
    bool start(int argc, char const** argv);

private:
    static void addConfigurationOptions(IConfiguration& configuration);
    bool        parseCommandLine(int argc, char const** argv);
    bool        parseConfigurationFile();

    const std::string               m_name;
    Configuration                   m_configCommandLine;
    Configuration                   m_configuration;
    ServiceComponentsExecutionGroup m_components;
};

}  // namespace sugo
