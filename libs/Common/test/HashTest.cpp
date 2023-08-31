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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <array>
#include <limits>

#include "Common/Hash.hpp"

using namespace sugo::common;

class HashTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(HashTest, Fnv1aValues)
{
    EXPECT_EQ(Fnv1a<uint32_t>::hash(""), 0x811c9dc5);
    EXPECT_EQ(Fnv1a<uint32_t>::hash(" "), 0x250c8f7f);
    EXPECT_EQ(Fnv1a<uint32_t>::hash("That's a test"), 0xb2704f3f);
}

TEST_F(HashTest, Fnv1Values)
{
    EXPECT_EQ(Fnv1<uint32_t>::hash(""), 0x811c9dc5);
    EXPECT_EQ(Fnv1<uint32_t>::hash(" "), 0x050c5d3f);
    EXPECT_EQ(Fnv1<uint32_t>::hash("That's a test"), 0x51418f23);
}

TEST_F(HashTest, Fnv1aLiteralValues)
{
    EXPECT_EQ(""_hash, 0x811c9dc5);
    EXPECT_EQ(" "_hash, 0x250c8f7f);
    EXPECT_EQ("That's a test"_hash, 0xb2704f3f);
}

TEST_F(HashTest, Fnv1CombinedHashes)
{
    EXPECT_EQ(Fnv1a<uint32_t>::combine(0, 0), 0);
    EXPECT_NE(Fnv1a<uint32_t>::combine(1, 0), 0);  // must not be 0!
    EXPECT_NE(Fnv1a<uint32_t>::combine(0, 1), 0);  // must not be 0!
    EXPECT_NE(Fnv1a<uint32_t>::combine(0, std::numeric_limits<uint32_t>::max()),
              0);  // must not be 0!
    EXPECT_NE(Fnv1a<uint32_t>::combine(std::numeric_limits<uint32_t>::max(), 0),
              0);  // must not be 0!
    EXPECT_LT(Fnv1a<uint32_t>::combine(std::numeric_limits<uint32_t>::max(),
                                       std::numeric_limits<uint32_t>::max()),
              std::numeric_limits<uint32_t>::max());
}

TEST_F(HashTest, Fnv1CombinedHashesInjective)
{
    EXPECT_NE(Fnv1a<uint32_t>::combine(0, 1), Fnv1a<uint32_t>::combine(1, 0));
    EXPECT_NE(Fnv1a<uint32_t>::combine(0, std::numeric_limits<uint32_t>::max()),
              Fnv1a<uint32_t>::combine(std::numeric_limits<uint32_t>::max(), 0));
    EXPECT_NE(Fnv1a<uint32_t>::combine(Fnv1<uint32_t>::hash("1"), Fnv1<uint32_t>::hash("2")),
              Fnv1a<uint32_t>::combine(Fnv1<uint32_t>::hash("2"), Fnv1<uint32_t>::hash("1")));
}