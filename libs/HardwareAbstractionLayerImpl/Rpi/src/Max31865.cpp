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

#include "HardwareAbstractionLayer/Max31865.hpp"
#include "Common/Logger.hpp"
#include "HardwareAbstractionLayer/HalTypes.hpp"

#include <array>
#include <limits>
#include <thread>

using namespace sugo::hal;

namespace
{
constexpr int32_t RefResistanceOhm = 43000;  // milli Ohm
constexpr uint8_t WriteMask        = 0x80u;
constexpr int32_t FaultMask        = 0x0001;
constexpr int32_t Factor           = 32768;  // 2^15
constexpr int32_t HalfFactor       = Factor / 2;

constexpr char Me[] = "MAX31865: ";

enum Register
{
    Config                = 0x00,
    RtdMsb                = 0x01,
    RtdLsb                = 0x02,
    HighFaultThresholdMsb = 0x03,
    HighFaultThresholdLsb = 0x04,
    LowFaultThresholdMsb  = 0x05,
    LowFaultThresholdLsb  = 0x06,
    FaultStatus           = 0x07
};

constexpr uint8_t ConfigRegisterBias               = 0x80u;
constexpr uint8_t ConfigRegisterConversionModeAuto = 0x40u;
constexpr uint8_t ConfigRegisterFaultStatusClear   = 0x02u;

struct ResistanceTemperature
{
    int32_t resistance;
    int16_t temperature;
};
const std::array<ResistanceTemperature, 301> ResistanceToTemperatureLookUpTable = {
    {{8031, -50},  {8070, -49},  {8110, -48},  {8150, -47},  {8189, -46},  {8229, -45},
     {8269, -44},  {8308, -43},  {8348, -42},  {8387, -41},  {8427, -40},  {8467, -39},
     {8506, -38},  {8546, -37},  {8585, -36},  {8625, -35},  {8664, -34},  {8704, -33},
     {8743, -32},  {8783, -31},  {8822, -30},  {8862, -29},  {8901, -28},  {8940, -27},
     {8980, -26},  {9019, -25},  {9059, -24},  {9098, -23},  {9137, -22},  {9177, -21},
     {9216, -20},  {9255, -19},  {9295, -18},  {9334, -17},  {9373, -16},  {9412, -15},
     {9452, -14},  {9491, -13},  {9530, -12},  {9569, -11},  {9609, -10},  {9648, -9},
     {9687, -8},   {9726, -7},   {9765, -6},   {9804, -5},   {9844, -4},   {9883, -3},
     {9922, -2},   {9961, -1},   {10000, 0},   {10039, 1},   {10078, 2},   {10117, 3},
     {10156, 4},   {10195, 5},   {10234, 6},   {10273, 7},   {10312, 8},   {10351, 9},
     {10390, 10},  {10429, 11},  {10468, 12},  {10507, 13},  {10546, 14},  {10585, 15},
     {10624, 16},  {10663, 17},  {10702, 18},  {10740, 19},  {10779, 20},  {10818, 21},
     {10857, 22},  {10896, 23},  {10935, 24},  {10973, 25},  {11012, 26},  {11051, 27},
     {11090, 28},  {11129, 29},  {11167, 30},  {11206, 31},  {11245, 32},  {11283, 33},
     {11322, 34},  {11361, 35},  {11400, 36},  {11438, 37},  {11477, 38},  {11515, 39},
     {11554, 40},  {11593, 41},  {11631, 42},  {11670, 43},  {11708, 44},  {11747, 45},
     {11786, 46},  {11824, 47},  {11863, 48},  {11901, 49},  {11940, 50},  {11978, 51},
     {12017, 52},  {12055, 53},  {12094, 54},  {12132, 55},  {12171, 56},  {12209, 57},
     {12247, 58},  {12286, 59},  {12324, 60},  {12363, 61},  {12401, 62},  {12439, 63},
     {12478, 64},  {12516, 65},  {12554, 66},  {12593, 67},  {12631, 68},  {12669, 69},
     {12708, 70},  {12746, 71},  {12784, 72},  {12822, 73},  {12861, 74},  {12899, 75},
     {12937, 76},  {12975, 77},  {13013, 78},  {13052, 79},  {13090, 80},  {13128, 81},
     {13166, 82},  {13204, 83},  {13242, 84},  {13280, 85},  {13318, 86},  {13357, 87},
     {13395, 88},  {13433, 89},  {13471, 90},  {13509, 91},  {13547, 92},  {13585, 93},
     {13623, 94},  {13661, 95},  {13699, 96},  {13737, 97},  {13775, 98},  {13813, 99},
     {13851, 100}, {13888, 101}, {13926, 102}, {13964, 103}, {14002, 104}, {14040, 105},
     {14078, 106}, {14116, 107}, {14154, 108}, {14191, 109}, {14229, 110}, {14267, 111},
     {14305, 112}, {14343, 113}, {14380, 114}, {14418, 115}, {14456, 116}, {14494, 117},
     {14531, 118}, {14569, 119}, {14607, 120}, {14644, 121}, {14682, 122}, {14720, 123},
     {14757, 124}, {14795, 125}, {14833, 126}, {14870, 127}, {14908, 128}, {14946, 129},
     {14983, 130}, {15021, 131}, {15058, 132}, {15096, 133}, {15133, 134}, {15171, 135},
     {15208, 136}, {15246, 137}, {15283, 138}, {15321, 139}, {15358, 140}, {15396, 141},
     {15433, 142}, {15471, 143}, {15508, 144}, {15546, 145}, {15583, 146}, {15620, 147},
     {15658, 148}, {15695, 149}, {15733, 150}, {15770, 151}, {15807, 152}, {15845, 153},
     {15882, 154}, {15919, 155}, {15956, 156}, {15994, 157}, {16031, 158}, {16068, 159},
     {16105, 160}, {16143, 161}, {16180, 162}, {16217, 163}, {16254, 164}, {16291, 165},
     {16329, 166}, {16366, 167}, {16403, 168}, {16440, 169}, {16477, 170}, {16514, 171},
     {16551, 172}, {16589, 173}, {16626, 174}, {16663, 175}, {16700, 176}, {16737, 177},
     {16774, 178}, {16811, 179}, {16848, 180}, {16885, 181}, {16922, 182}, {16959, 183},
     {16996, 184}, {17033, 185}, {17070, 186}, {17107, 187}, {17143, 188}, {17180, 189},
     {17217, 190}, {17254, 191}, {17291, 192}, {17328, 193}, {17365, 194}, {17402, 195},
     {17438, 196}, {17475, 197}, {17512, 198}, {17549, 199}, {17586, 200}, {17622, 201},
     {17659, 202}, {17696, 203}, {17733, 204}, {17769, 205}, {17806, 206}, {17843, 207},
     {17879, 208}, {17916, 209}, {17953, 210}, {17989, 211}, {18026, 212}, {18063, 213},
     {18099, 214}, {18136, 215}, {18172, 216}, {18209, 217}, {18246, 218}, {18282, 219},
     {18319, 220}, {18355, 221}, {18392, 222}, {18428, 223}, {18465, 224}, {18501, 225},
     {18538, 226}, {18574, 227}, {18611, 228}, {18647, 229}, {18684, 230}, {18720, 231},
     {18756, 232}, {18793, 233}, {18829, 234}, {18866, 235}, {18902, 236}, {18938, 237},
     {18975, 238}, {19011, 239}, {19047, 240}, {19084, 241}, {19120, 242}, {19156, 243},
     {19192, 244}, {19229, 245}, {19265, 246}, {19301, 247}, {19337, 248}, {19374, 249},
     {19410, 250}}};
}  // namespace

bool Max31865::init()
{
    const ByteBuffer config{ConfigRegisterBias | ConfigRegisterConversionModeAuto |
                            ConfigRegisterFaultStatusClear};
    if (!writeSpiRegister(Register::Config, config))
    {
        LOG(error) << Me << "Failed to write config";
        return false;
    }

    ByteBuffer allRegister(8u);
    if (!readSpiData(Register::Config, allRegister))
    {
        LOG(error) << Me << "Failed to read all register";
        return false;
    }

    if (allRegister.at(Register::Config) != (ConfigRegisterBias | ConfigRegisterConversionModeAuto))
    {
        LOG(error) << Me << "Failed to config the device";
        return false;
    }

    if (allRegister.at(Register::FaultStatus) != 0)
    {
        LOG(error) << Me << "Unexpected fault status: " << allRegister.at(Register::FaultStatus);
        return false;
    }

    return true;
}

int16_t Max31865::getTemperature()
{
    ByteBuffer rtd{0, 0};
    readSpiData(Register::RtdMsb, rtd);
    if ((FaultMask & rtd.at(1)) != 0)
    {
        LOG(error) << Me << "Failed to retrieve new temperature";
        return std::numeric_limits<int16_t>::min();
    }
    // Note: Fault bit is shifted out (>>1)!
    int16_t adcCode = (static_cast<int16_t>(rtd.at(1)) | static_cast<int16_t>(rtd.at(0)) << 8);
    adcCode >>= 1;
    const int32_t resistance =
        ((static_cast<int32_t>(adcCode) * RefResistanceOhm) + HalfFactor) / Factor;
    const auto entry = std::find_if(
        ResistanceToTemperatureLookUpTable.cbegin(), ResistanceToTemperatureLookUpTable.cend(),
        [&resistance](const ResistanceTemperature& resistanceTemperature) {
            return (resistance <= resistanceTemperature.resistance);
        });

    if (entry != ResistanceToTemperatureLookUpTable.cend())
    {
        return entry->temperature;
    }
    else
    {
        LOG(warning) << Me << "Failed to calculate temperature value";
        return std::numeric_limits<int16_t>::max();
    }
}

bool Max31865::writeSpiRegister(uint8_t startRegister, const ByteBuffer& writeData)
{
    bool success = m_ioCs.setState(IGpioPin::State::High);
    if (success)
    {
        (void)m_spi.writeByte(startRegister | WriteMask);
        for (unsigned i = 0; i < writeData.size(); i++)
        {
            (void)m_spi.writeByte(writeData[i]);
        }
        success = m_ioCs.setState(IGpioPin::State::Low) && success;
    }
    return success;
}

bool Max31865::readSpiData(uint8_t startRegister, ByteBuffer& readData)
{
    bool success = m_ioCs.setState(IGpioPin::State::High);
    if (success)
    {
        (void)m_spi.writeByte(startRegister);
        for (unsigned i = 0; i < readData.size(); i++)
        {
            readData[i] = m_spi.readByte();
        }
        success = m_ioCs.setState(IGpioPin::State::Low) && success;
    }
    return success;
}
