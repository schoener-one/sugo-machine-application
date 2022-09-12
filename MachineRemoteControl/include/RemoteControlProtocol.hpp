///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-09-13
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

namespace sugo::remote_control::protocol
{
inline static const std::string IdResult{"result"};
inline static const std::string IdSpeed{"speed"};
inline static const std::string IdState{"state"};
inline static const std::string IdType{"type"};
inline static const std::string IdCommand{"command"};
inline static const std::string IdTypeResponseRequest{"response-request"};
inline static const std::string IdTypeResponseCommand{"response-command"};
inline static const std::string IdTypeRequestCommand{"request-command"};
inline static const std::string IdTypeRequestState{"request-state"};
inline static const std::string IdTypeResponseState{"response-request-state"};
inline static const std::string IdErrorReason{"reason"};
inline static const std::string IdResultError{"error"};
inline static const std::string IdResultSuccess{"success"};
inline static const std::string IdErrorCommandUnsupported{"error-command-unsupported"};
inline static const std::string IdErrorRequestUnsupported{"error-request-unsupported"};
inline static const std::string IdErrorTypeInvalid{"error-type-invalid"};
}  // namespace sugo::remote_control::protocol