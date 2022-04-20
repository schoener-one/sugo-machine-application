///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-16
 */
///////////////////////////////////////////////////////////////////////////////

#include "AdcFilter.hpp"
#include "Logger.hpp"

#include <array>
#include <limits>
#include <assert.h>

using ResistanceTranslation = struct
{
    int resistance;
    int temperature;
};

namespace
{
constexpr std::array<ResistanceTranslation, 331> s_resistanceToTemperatureList{
    {{1733200, -30}, {1630408, -29}, {1534477, -28}, {1444903, -27}, {1361220, -26}, {1283000, -25},
     {1209327, -24}, {1140424, -23}, {1075949, -22}, {1015588, -21}, {959050, -20},  {906012, -19},
     {856288, -18},  {809651, -17},  {765887, -16},  {724800, -15},  {685651, -14},  {648893, -13},
     {614365, -12},  {581917, -11},  {551410, -10},  {522691, -9},   {495667, -8},   {470229, -7},
     {446271, -6},   {423700, -5},   {402056, -4},   {381666, -3},   {362449, -2},   {344330, -1},
     {327240, 0},    {311040, 1},    {295751, 2},    {281316, 3},    {267682, 4},    {254800, 5},
     {242583, 6},    {231032, 7},    {220108, 8},    {209772, 9},    {199990, 10},   {190558, 11},
     {181632, 12},   {173182, 13},   {165180, 14},   {157600, 15},   {150425, 16},   {143623, 17},
     {137173, 18},   {131053, 19},   {125245, 20},   {119658, 21},   {114356, 22},   {109322, 23},
     {104542, 24},   {100000, 25},   {95819, 26},    {91839, 27},    {88049, 28},    {84440, 29},
     {81000, 30},    {77624, 31},    {74409, 32},    {71347, 33},    {68430, 34},    {65650, 35},
     {62984, 36},    {60442, 37},    {58018, 38},    {55706, 39},    {53500, 40},    {51371, 41},
     {49339, 42},    {47400, 43},    {45548, 44},    {43780, 45},    {42056, 46},    {40409, 47},
     {38837, 48},    {37335, 49},    {35900, 50},    {34616, 51},    {33386, 52},    {32206, 53},
     {31075, 54},    {29990, 55},    {28905, 56},    {27866, 57},    {26870, 58},    {25915, 59},
     {25000, 60},    {24110, 61},    {23257, 62},    {22438, 63},    {21653, 64},    {20900, 65},
     {20174, 66},    {19477, 67},    {18809, 68},    {18167, 69},    {17550, 70},    {16946, 71},
     {16366, 72},    {15809, 73},    {15274, 74},    {14760, 75},    {14281, 76},    {13821, 77},
     {13377, 78},    {12951, 79},    {12540, 80},    {12135, 81},    {11745, 82},    {11369, 83},
     {11008, 84},    {10660, 85},    {10324, 86},    {10001, 87},    {9690, 88},     {9389, 89},
     {9100, 90},     {8817, 91},     {8544, 92},     {8282, 93},     {8028, 94},     {7784, 95},
     {7554, 96},     {7332, 97},     {7117, 98},     {6910, 99},     {6710, 100},    {6527, 101},
     {6349, 102},    {6177, 103},    {6011, 104},    {5850, 105},    {5683, 106},    {5522, 107},
     {5366, 108},    {5216, 109},    {5070, 110},    {4929, 111},    {4793, 112},    {4661, 113},
     {4533, 114},    {4410, 115},    {4291, 116},    {4175, 117},    {4063, 118},    {3955, 119},
     {3850, 120},    {3741, 121},    {3636, 122},    {3534, 123},    {3435, 124},    {3340, 125},
     {3255, 126},    {3173, 127},    {3093, 128},    {3015, 129},    {2940, 130},    {2863, 131},
     {2789, 132},    {2717, 133},    {2648, 134},    {2580, 135},    {2514, 136},    {2451, 137},
     {2389, 138},    {2329, 139},    {2271, 140},    {2214, 141},    {2158, 142},    {2104, 143},
     {2051, 144},    {2000, 145},    {1951, 146},    {1904, 147},    {1858, 148},    {1813, 149},
     {1770, 150},    {1732, 151},    {1695, 152},    {1659, 153},    {1623, 154},    {1589, 155},
     {1552, 156},    {1516, 157},    {1481, 158},    {1447, 159},    {1414, 160},    {1381, 161},
     {1349, 162},    {1318, 163},    {1288, 164},    {1259, 165},    {1230, 166},    {1202, 167},
     {1175, 168},    {1148, 169},    {1122, 170},    {1096, 171},    {1070, 172},    {1045, 173},
     {1021, 174},    {997, 175},     {976, 176},     {955, 177},     {935, 178},     {915, 179},
     {896, 180},     {875, 181},     {855, 182},     {835, 183},     {816, 184},     {797, 185},
     {781, 186},     {765, 187},     {749, 188},     {734, 189},     {719, 190},     {703, 191},
     {687, 192},     {672, 193},     {657, 194},     {643, 195},     {630, 196},     {618, 197},
     {606, 198},     {594, 199},     {582, 200},     {572, 201},     {562, 202},     {552, 203},
     {542, 204},     {533, 205},     {523, 206},     {512, 207},     {502, 208},     {493, 209},
     {483, 210},     {473, 211},     {464, 212},     {455, 213},     {446, 214},     {437, 215},
     {428, 216},     {420, 217},     {412, 218},     {404, 219},     {396, 220},     {388, 221},
     {381, 222},     {374, 223},     {367, 224},     {360, 225},     {353, 226},     {347, 227},
     {340, 228},     {334, 229},     {328, 230},     {322, 231},     {316, 232},     {310, 233},
     {305, 234},     {299, 235},     {294, 236},     {289, 237},     {284, 238},     {279, 239},
     {274, 240},     {269, 241},     {264, 242},     {260, 243},     {255, 244},     {251, 245},
     {246, 246},     {242, 247},     {238, 248},     {234, 249},     {230, 250},     {226, 251},
     {222, 252},     {219, 253},     {215, 254},     {211, 255},     {208, 256},     {205, 257},
     {201, 258},     {198, 259},     {195, 260},     {191, 261},     {188, 262},     {185, 263},
     {182, 264},     {179, 265},     {177, 266},     {174, 267},     {171, 268},     {168, 269},
     {166, 270},     {163, 271},     {160, 272},     {158, 273},     {156, 274},     {153, 275},
     {151, 276},     {148, 277},     {146, 278},     {144, 279},     {142, 280},     {140, 281},
     {138, 282},     {135, 283},     {133, 284},     {131, 285},     {130, 286},     {128, 287},
     {126, 288},     {124, 289},     {122, 290},     {120, 291},     {119, 292},     {117, 293},
     {115, 294},     {113, 295},     {112, 296},     {110, 297},     {109, 298},     {107, 299},
     {106, 300}}};

constexpr int translateRawToVoltage(int rawValue, int refVoltage)
{
    constexpr int64_t gain    = 1;
    constexpr int64_t div     = 0x80000000;  // 2^31
    const int64_t     voltage = (refVoltage * static_cast<int64_t>(rawValue)) / (gain * div);
    assert(voltage < std::numeric_limits<int>::max());
    return static_cast<int>(voltage);
}

constexpr int translateVoltageToVoltageDividerResistance(int voltage, int refVoltage,
                                                         int resistanceMain, int resistanceDiv)
{
    int resistance = std::numeric_limits<int>::max();
    if (refVoltage != 0)
    {
        resistance = ((refVoltage - voltage) * (resistanceMain + resistanceDiv)) / refVoltage;
    }
    return resistance;
}

constexpr int translateResistanceToTemperature(int resistance)
{
    for (auto& item : s_resistanceToTemperatureList)
    {
        if (item.resistance < resistance)
        {
            return item.temperature;
        }
    }
    return std::numeric_limits<int>::min();
}
}  // namespace

using namespace sugo::hal;

bool AdcFilter::init(const IConfiguration& configuration)
{
    m_refVoltage     = configuration.getOption("ref-voltage").get<int>();
    m_mainResistance = configuration.getOption("resistance").get<unsigned>();
    m_divResistance  = configuration.getOption("divider-resistance").get<unsigned>();
    const std::string translationName = configuration.getOption("translation").get<std::string>();

    if (translationName == "ntc-100k-temperature")
    {
        m_translation = AdcValueTranslation::Ntc100k;
    }
    else if (translationName == "voltage")
    {
        m_translation = AdcValueTranslation::Voltage;
    }
    else if (translationName == "voltage-divider-resistance")
    {
        m_translation = AdcValueTranslation::Resistance;
    }
    else
    {
        m_translation = AdcValueTranslation::Ident;
    }

    LOG(debug) << getId() << ": ref-voltage set to " << m_refVoltage;
    LOG(debug) << getId() << ": main resistance set to " << m_mainResistance;
    LOG(debug) << getId() << ": divider resistance set to " << m_divResistance;
    LOG(debug) << getId() << ": translation set to " << translationName;

    return true;
}

IAdcFilter::ValueType AdcFilter::convert(RawValueType rawAdcValue) const
{
    RawValueType value = 0;
    Unit         unit  = Unit::Unknown;
    switch (m_translation)
    {
        case AdcValueTranslation::Ident:
            value = rawAdcValue;
            unit  = Unit::Unknown;
            break;
        case AdcValueTranslation::Voltage:
            value = translateRawToVoltage(rawAdcValue, m_refVoltage);
            unit  = Unit::Volt;
            break;
        case AdcValueTranslation::Resistance:
            value = translateVoltageToVoltageDividerResistance(
                translateRawToVoltage(rawAdcValue, m_refVoltage), m_refVoltage, m_mainResistance,
                m_divResistance);
            unit = Unit::Ohm;
            break;
        case AdcValueTranslation::Ntc100k:
        {
            value = translateResistanceToTemperature(translateVoltageToVoltageDividerResistance(
                translateRawToVoltage(rawAdcValue, m_refVoltage), m_refVoltage, m_mainResistance,
                m_divResistance));
            unit  = Unit::Celcius;
            break;
        }
    }
    return ValueType(value, unit);
}