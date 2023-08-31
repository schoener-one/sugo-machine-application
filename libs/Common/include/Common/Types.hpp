///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2023
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

#include <cassert>
#include <jsonrpcpp/json.hpp>

#include "Common/Logger.hpp"

/**
 * @brief Stops the execution of code with an fatal error
 * and jumps to an fatal error handler (Release) or throws
 * an assert (Debug).
 * @todo Handle Release build asserts here!
 *
 */
#define ASSERT_NOT_REACHABLE               \
    LOG(fatal) << "Should not be reached"; \
    assert(false)

namespace sugo::common
{
/// @brief JSON data representation type
using Json = nlohmann::json;
}  // namespace sugo::common
