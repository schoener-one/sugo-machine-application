///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
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

#include <string>

#include "Common/Configuration.hpp"

namespace sugo::machine_application
{
/// @brief Class representing the machine application.
class MachineApplication final
{
public:
    /**
     * @brief Construct a new machine application instance.
     *
     * @param appName Name of the application instance.
     */
    explicit MachineApplication(std::string appName);

    /**
     * @brief Starts the application with the command line and file configuration.
     * The start call will not return until the application has been stopped!
     *
     * @param argc Argument count.
     * @param argv Pointer to a vector of string passed as arguments.
     * @return true If the application could be started successfully.
     * @return false If the application could not be started successfully.
     */
    bool start(int argc, char const** argv);

private:
    /// @brief Add configuration options to the configuration.
    void addConfigurationOptions();

    /**
     * @brief Parses the command-line arguments.
     *
     * @param argc Number of arguments.
     * @param argv Vector of strings received from the call of the application.
     * @return true If all arguments could be parsed successfully.
     * @return false If not all arguments could be parsed successfully.
     */
    bool parseCommandLine(int argc, char const** argv);

    /**
     * @brief Parses the specified configuration file.
     *
     * @return true If all arguments could be parsed successfully.
     * @return false If not all arguments could be parsed successfully.
     */
    bool parseConfigurationFile();

    const std::string     m_name;               ///< Name of this application.
    common::Configuration m_configCommandLine;  ///< Command-line configuration set.
    common::Configuration m_configuration;      ///< Application configuration set.
};

}  // namespace sugo::machine_application
