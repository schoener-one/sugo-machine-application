///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   10.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef STREAMBUFFER_HPP_
#define STREAMBUFFER_HPP_

#include <boost/asio/streambuf.hpp>

namespace sugo
{
/**
 * Stream buffer class with fix buffer size.
 */
class StreamBuffer : public boost::asio::streambuf
{
public:
    /// Default size of the send/receive buffers.
    constexpr static unsigned MaxBufferSize = 2048;

    StreamBuffer() : boost::asio::streambuf(MaxBufferSize) {}
};

}  // namespace sugo

#endif  // STREAMBUFFER_HPP_
