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

#include <boost/asio/streambuf.hpp>

namespace sugo::message_broker
{
/**
 * @brief Generic class represents a Stream buffer with a fixed buffer size.
 *
 * @tparam MaxBufferSizeT
 */
template <std::size_t MaxBufferSizeT>
class GenericStreamBuffer : public boost::asio::streambuf
{
public:
    /// Default size of the send/receive buffers.
    constexpr static unsigned MaxBufferSize = MaxBufferSizeT;

    GenericStreamBuffer() : boost::asio::streambuf(MaxBufferSize)
    {
    }

    /// @brief Copy constructor
    GenericStreamBuffer(const GenericStreamBuffer<MaxBufferSizeT>&) = delete;

    /**
     * @brief Move constructor.
     *
     * @warning This could cause unexpected behaviour, because not all members are movable, so use
     * it with care!
     */
    GenericStreamBuffer(GenericStreamBuffer<MaxBufferSizeT>&&)
    {
    }

    /// @brief Copy operator.
    /// @return This object.
    GenericStreamBuffer<MaxBufferSizeT>& operator=(const GenericStreamBuffer<MaxBufferSizeT>&) =
        delete;

    /**
     * @brief Move operator.
     *
     * @param streamBuffer Stream buffer object to move to this.
     * @return This object.
     * @warning This could cause unexpected behaviour, because not all members are movable, so use
     * it with care!
     */
    GenericStreamBuffer<MaxBufferSizeT>& operator=(
        GenericStreamBuffer<MaxBufferSizeT>&& streamBuffer)
    {
        *this = std::move(streamBuffer);
        return *this;
    }
};

/// @brief Fix size stream buffer type.
using StreamBuffer = GenericStreamBuffer<2048u>;
}  // namespace sugo::message_broker
