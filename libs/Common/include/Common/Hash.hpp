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

#include <cstdint>
#include <string>

namespace sugo::common
{
/// @brief Base class for FNV1(a) algorithm.
template <typename HashT = uint32_t>
struct BaseFnv
{
    constexpr static HashT defaultOffsetBasis = 0x811C9DC5;
    constexpr static HashT prime              = 0x01000193;
};

/**
 * @brief Generic class for creating FNV1 hashes.
 *
 * @tparam HashT Hash type.
 */
template <typename HashT = uint32_t>
struct Fnv1 : public BaseFnv<HashT>
{
    /**
     * @brief Creates a FNV1 hash from string.
     *
     * @param aString String to hash.
     * @param val Offset basis for algorithm.
     * @return The hash value.
     */
    constexpr static inline HashT hash(char const* const aString,
                                       const HashT       val = BaseFnv<HashT>::defaultOffsetBasis)
    {
        return (aString[0] == '\0') ? val
                                    : hash(&aString[1], (val * BaseFnv<HashT>::prime) ^
                                                            static_cast<HashT>(aString[0]));
    }

    /**
     * @brief Combines two hashes to one new hash.
     *
     * @param hashA Hash value A to combine.
     * @param hashB Hash value B to combine.
     * @return New combined hash value.
     */
    constexpr static inline HashT combine(HashT hashA, HashT hashB)
    {
        return hashA * BaseFnv<HashT>::prime + hashB;
    }
};

/**
 * @brief Generic class for creating FNV1a hashes.
 *
 * @tparam HashT Hash type.
 */
template <typename HashT = uint32_t>
struct Fnv1a : public BaseFnv<HashT>
{
    /**
     * @brief Creates a FNV1a hash from string.
     *
     * @param aString String to hash.
     * @param val Offset basis for algorithm.
     * @return The hash value.
     */
    constexpr static inline HashT hash(char const* const aString,
                                       const HashT       val = BaseFnv<HashT>::defaultOffsetBasis)
    {
        return (aString[0] == '\0') ? val
                                    : hash(&aString[1], (val ^ static_cast<HashT>(aString[0])) *
                                                            BaseFnv<HashT>::prime);
    }

    /**
     * @brief Combines two hashes to one new hash.
     *
     * @param hashA Hash value A to combine.
     * @param hashB Hash value B to combine.
     * @return New combined hash value.
     */
    constexpr static inline HashT combine(HashT hashA, HashT hashB)
    {
        return hashA * BaseFnv<HashT>::prime + hashB;
    }
};
}  // namespace sugo::common

constexpr std::uint32_t operator"" _hash(const char* value,
                                         std::size_t)  // TODO use the length here!
{
    return sugo::common::Fnv1a<uint32_t>::hash(value);
}
