/*
 * CoffeeAutomatApplication.hpp
 *
 *  Created on: 01.12.2019
 *      Author: denis
 */

#ifndef COFFEEAUTOMATAPPLICATION_HPP_
#define COFFEEAUTOMATAPPLICATION_HPP_

#include <memory>

#include "Configuration.hpp"

namespace moco
{
/**
 * CoffeeAutomat application class.
 */
class CoffeeAutomatApplication final
{
public:
    CoffeeAutomatApplication();

    /**
     * Starts the application with command line arguments.
     * @param argc Argument count.
     * @param argv Pointer to a vector of string passed as arguments.
     * @return     0 if the application run successfully, value smaller
     *             than 0 to indicate an error.
     */
    bool start(int argc, char const** argv);

private:
    bool parseCommandLine(int argc, char const** argv);
    bool parseConfigurationFile();

    Configuration m_configuration;
};

}  // namespace moco

#endif /* COFFEEAUTOMATAPPLICATION_HPP_ */
