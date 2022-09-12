///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2022 by Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   18.09.2022
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Globals.hpp"
#include "MessageProtocol.hpp"

#include <Command.pb.h>

namespace sugo::message
{
inline static message::CommandResponse createResponse(
    const message::Command& command, const Json& response = Json(),
    const message::CommandResponse_Result result = message::CommandResponse_Result_SUCCESS)
{
    message::CommandResponse commandResponse;
    commandResponse.set_id(command.id());
    commandResponse.set_result(result);
    commandResponse.set_response(response.dump());
    return commandResponse;
}

inline static message::CommandResponse createErrorResponse(
    const message::Command& command,
    const Json& errorMessage = Json({{protocol::IdErrorReason, protocol::IdErrorUnspecified}}),
    message::CommandResponse_Result errorCode = message::CommandResponse_Result_ERROR)
{
    return createResponse(command, errorMessage, errorCode);
}

inline static message::CommandResponse createUnsupportedCommandResponse(
    const message::Command& command)
{
    return createErrorResponse(
        command, Json({{protocol::IdErrorReason, protocol::IdErrorCommandUnsupported}}),
        message::CommandResponse_Result_INVALID_COMMAND);
}
}  // namespace sugo::message
