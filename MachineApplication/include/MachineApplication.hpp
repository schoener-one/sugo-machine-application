///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   01.12.2019
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef MACHINEAPPLICATION_HPP_
#define MACHINEAPPLICATION_HPP_

#include <map>
#include <memory>
#include <typeindex>

#include "Configuration.hpp"
#include "Server.hpp"

namespace moco
{
/**
 * CoffeeAutomat application class.
 */
class MachineApplication final
{
public:
    MachineApplication();

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
    bool run();

    Configuration                        m_configCommandLine;
    Configuration                        m_configuration;
    std::map<std::type_index, IOContext> m_ioContexts;
};

}  // namespace moco

#endif /* MACHINEAPPLICATION_HPP_ */
