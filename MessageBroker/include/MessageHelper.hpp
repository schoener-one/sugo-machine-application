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
/**
 * @brief Creates a successful command response.
 *
 * @param command  Command this response is based on.
 * @param response Command response payload.
 * @param result   Command execution result.
 * @return The command response.
 */
inline static message::CommandResponse createCommandResponse(
    const message::Command& command, const Json& response = Json(),
    const message::CommandResponse_Result result = message::CommandResponse_Result_SUCCESS)
{
    message::CommandResponse commandResponse;
    commandResponse.set_id(command.id());
    commandResponse.set_result(result);
    commandResponse.set_response(response.dump());
    return commandResponse;
}

/**
 * @brief Creates a error command response.
 * Should be used if the command could not be executed successfully.
 *
 * @param command      Command this response is based on.
 * @param errorMessage Command error response payload.
 * @param errorCode    Command execution result.
 * @return The command error response.
 */
inline static message::CommandResponse createErrorCommandResponse(
    const message::Command& command,
    const Json& errorMessage = Json({{protocol::IdErrorReason, protocol::IdErrorUnspecified}}),
    message::CommandResponse_Result errorCode = message::CommandResponse_Result_ERROR)
{
    return createCommandResponse(command, errorMessage, errorCode);
}

/**
 * @brief Creates a unsupported command response.
 * Should be created if the requested command is not supported.
 *
 * @param command
 * @return message::CommandResponse
 */
inline static message::CommandResponse createUnsupportedCommandResponse(
    const message::Command& command)
{
    LOG(warning) << "Received unhandled command: " << command.name();
    return createErrorCommandResponse(
        command, Json({{protocol::IdErrorReason, protocol::IdErrorCommandUnsupported}}),
        message::CommandResponse_Result_INVALID_COMMAND);
}
}  // namespace sugo::message
