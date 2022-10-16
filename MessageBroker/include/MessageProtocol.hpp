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

namespace sugo::message::protocol
{
inline static const std::string IdState{"state"};
inline static const std::string IdErrorReason{"reason"};
inline static const std::string IdErrorUnspecified{"error-unspecified"};
inline static const std::string IdErrorCommandUnsupported{"error-command-unsupported"};
inline static const std::string IdErrorInvalidState{"error-state-invalid"};
inline static const std::string IdErrorRequestTooMuchRequests{"error-state-too-much-requests"};
inline static const std::string IdErrorCommandParameterInvalid{"error-command-parameter-invalid"};
inline static const std::string IdErrorRequestUnsupported{"error-request-unsupported"};
}  // namespace sugo::protocol