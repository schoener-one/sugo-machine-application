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

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <cassert>
#include <jsonrpcpp/json.hpp>

#include "Logger.hpp"

#define ASSERT_NOT_REACHABLE               \
    LOG(fatal) << "Should not be reached"; \
    assert(false)

namespace sugo
{
using Json = nlohmann::json;
}
