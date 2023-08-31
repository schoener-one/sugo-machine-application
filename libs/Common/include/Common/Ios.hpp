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

#include <iomanip>
#include <ios>

namespace sugo::common::ios
{
template <typename _HexValueT>
struct _HexValue
{
    _HexValueT value;
};

template <typename _HexValueT>
inline _HexValue<_HexValueT> hex(_HexValueT n)
{
    return {n};
}

template <typename _CharT, typename _Traits, typename _HexValueT>
inline std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& os,
                                                       _HexValue<_HexValueT>                hex)
{
    constexpr std::size_t numOfCharsPerByte = 2u;
    os << std::hex << std::setw(sizeof(hex.value) * numOfCharsPerByte) << std::setfill('0')
       << hex.value;
    return os;
}
}  // namespace sugo::common::ios