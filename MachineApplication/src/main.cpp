///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   30.11.2019
 */
///////////////////////////////////////////////////////////////////////////////

#include <string>

#include "Globals.hpp"
#include "Logger.hpp"
#include "MachineApplication.hpp"

int main(int argc, char const **argv)
{
    sugo::Logger::init();
    sugo::MachineApplication application("SugoMachine");
    return application.start(argc, argv) ? 0 : -1;
}
