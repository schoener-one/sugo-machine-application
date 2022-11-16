///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-11-16
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iomanip>
#include <ios>

namespace sugo::ios
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
}  // namespace sugo::ios