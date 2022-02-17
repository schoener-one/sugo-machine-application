/*
 * Globals.hpp
 *
 *  Created on: 01.09.2019
 *      Author: denis
 */

#ifndef INCLUDE_GLOBALS_HPP_
#define INCLUDE_GLOBALS_HPP_
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

#endif /* INCLUDE_GLOBALS_HPP_ */
