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

namespace sugo::remote_control::id
{
inline static const std::string Result{"result"};
inline static const std::string Speed{"speed"};
inline static const std::string State{"state"};
inline static const std::string Type{"type"};
inline static const std::string Command{"command"};
inline static const std::string TypeResponseRequest{"response-request"};
inline static const std::string TypeResponseCommand{"response-command"};
inline static const std::string TypeRequestCommand{"request-command"};
inline static const std::string TypeRequestState{"request-state"};
inline static const std::string TypeResponseState{"response-state"};
inline static const std::string TypeNotificationState{"notification-state"};
inline static const std::string ErrorReason{"reason"};
inline static const std::string ResultError{"error"};
inline static const std::string ResultSuccess{"success"};
inline static const std::string ErrorCommandUnsupported{"error-command-unsupported"};
inline static const std::string ErrorRequestUnsupported{"error-request-unsupported"};
inline static const std::string ErrorTypeInvalid{"error-type-invalid"};
}  // namespace sugo::remote_control::id