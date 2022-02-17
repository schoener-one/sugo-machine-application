///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   09.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include "CommandId.hpp"

using namespace sugo;

std::ostream& sugo::operator<<(std::ostream& ostr, CommandId const& commandId)
{
    ostr << commandId.receiver << "." << commandId.command;
    return ostr;
}
