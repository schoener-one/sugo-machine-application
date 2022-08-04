///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   22.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ostream>
#include <string>

namespace sugo
{
/**
 * Class represents a command identifier.
 */
class CommandId
{
public:
    CommandId(const std::string& _receiverId, const std::string& _id)
        : receiver(_receiverId), command(_id)
    {
    }

    const std::string receiver;
    const std::string command;

    operator const std::string &() const
    {
        return command;
    }
};

std::ostream& operator<<(std::ostream& ostr, CommandId const& commandId);
}  // namespace sugo
