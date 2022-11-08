///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: Copyright 2020, Schoener-One
 *
 * @author: denis
 * @date:   10.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/asio/streambuf.hpp>

namespace sugo::message
{
/**
 * Stream buffer class with fix buffer size.
 *
 * @todo Replace this streambuffer by an own MessageBuffer implementation with fix buffer size!
 */
class StreamBuffer : public boost::asio::streambuf
{
public:
    /// Default size of the send/receive buffers.
    constexpr static unsigned MaxBufferSize = 2048;

    StreamBuffer() : boost::asio::streambuf(MaxBufferSize)
    {
    }
};

}  // namespace sugo::message
