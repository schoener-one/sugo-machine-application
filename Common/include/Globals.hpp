///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2019, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2019-09-01
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <jsonrpcpp/json.hpp>
#include "Logger.hpp"

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

namespace sugo
{
/// @brief JSON data representation
using Json = nlohmann::json;
}  // namespace sugo
