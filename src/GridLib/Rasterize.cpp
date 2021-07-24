//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-03-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Rasterize.hpp"
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    namespace
    {
        union Color
        {
            struct
            {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
            };
            uint32_t rgba;
        };

        void addRange(Chorasmia::IntervalMap<double, uint32_t>& map,
                      double fromHeight, double toHeight,
                      Color fromColor, Color toColor,
                      unsigned steps)
        {
            int32_t base[4] = {fromColor.r, fromColor.g, fromColor.b, fromColor.a};
            int32_t delta[4] = {int32_t(toColor.r - fromColor.r),
                                int32_t(toColor.g - fromColor.g),
                                int32_t(toColor.b - fromColor.b),
                                int32_t(toColor.a - fromColor.a)};
            for (int32_t i = 0; i < int32_t(steps); ++i)
            {
                Color c{.a = uint8_t(base[3] + (delta[3] * i) / int32_t(steps - 1)),
                    .b = uint8_t(base[2] + (delta[2] * i) / int32_t(steps - 1)),
                    .g = uint8_t(base[1] + (delta[1] * i) / int32_t(steps - 1)),
                    .r = uint8_t(base[0] + (delta[0] * i) / int32_t(steps - 1))};
                map.insert(fromHeight + (toHeight - fromHeight) * i / steps,
                           fromHeight + (toHeight - fromHeight) * (i + 1) / steps,
                           c.rgba);
            }
        }
    }

    Chorasmia::IntervalMap<double, uint32_t> makeMapGradient(
        double seaLevelMin,
        double seaLevelMax,
        double groundLevel1Max,
        double groundLevel2Max,
        double groundLevel3Max,
        double groundLevel4Max,
        double groundLevel5Max)
    {
        Chorasmia::IntervalMap<double, uint32_t> map(0xFFFF8E5Au);
        map.insert(seaLevelMax, 0xFFFCF2E9u);
        addRange(map, seaLevelMin, seaLevelMax, Color{.rgba = 0xFFFF8E5Au},
                 Color{.rgba = 0xFFFAEAD1u}, 10);
        addRange(map, seaLevelMax, groundLevel1Max, Color{.rgba = 0xFF74907Cu},
                 Color{.rgba = 0xFFD0E8D7u}, 10);
        addRange(map, groundLevel1Max, groundLevel2Max, Color{.rgba = 0xFFBDE8E5u},
                 Color{.rgba = 0xFF6F888Cu}, 10);
        addRange(map, groundLevel2Max, groundLevel3Max, Color{.rgba = 0xFF69848Cu},
                 Color{.rgba = 0xFF95B9D1u}, 10);
        addRange(map, groundLevel3Max, groundLevel4Max, Color{.rgba = 0xFF95A9C1u},
                 Color{.rgba = 0xFFE3DCDCu}, 10);
        addRange(map, groundLevel4Max, groundLevel5Max, Color{.rgba = 0xFFECC8A6u},
                 Color{.rgba = 0xFFFCF2E9u}, 10);

        // Extend the sea a little bit:
        map.insert(seaLevelMax, seaLevelMax + 0.25, 0xFFFAEAD1u);

        return map;
    }

    Chorasmia::IntervalMap<double, uint32_t> makeDefaultGradient2500()
    {
        return makeMapGradient(2500, 0, 500, 1000, 1500, 2000, 2500);
    }

    Chorasmia::IntervalMap<double, uint32_t> makeDefaultGradient9000()
    {
        return makeMapGradient(2500, 0, 1000, 2000, 3000, 5000, 9000);
    }

    namespace
    {
        enum class CardinalDirection
        {
            NORTH,
            EAST,
            SOUTH,
            WEST
        };

        CardinalDirection rotate(CardinalDirection dir, int times)
        {
            return CardinalDirection((int(dir) + times) % 4);
        }

        int getRotation(CardinalDirection srcDir, CardinalDirection dstDir)
        {
            return (int(dstDir) + 4 - int(srcDir)) % 4;
        }

        bool isFlipped(CardinalDirection rowDir, CardinalDirection colDir)
        {
            return (int(rowDir) + 4 - int(colDir)) % 4 == 1;
        }

        bool areOrthogonal(CardinalDirection a, CardinalDirection b)
        {
            return std::abs(int(a) - int(b)) % 2 == 1;
        }

        Chorasmia::Index2DMode
        getIndex2DMode(CardinalDirection srcRowDir, CardinalDirection srcColDir,
                       CardinalDirection dstRowDir, CardinalDirection dstColDir)
        {
            if (!areOrthogonal(srcRowDir, srcColDir))
                GRIDLIB_THROW("Source coordinate axes are non-orthogonal.");
            if (!areOrthogonal(dstRowDir, dstColDir))
                GRIDLIB_THROW("Destination coordinate axes are non-orthogonal.");

            if (auto rot = getRotation(dstRowDir, CardinalDirection::EAST))
            {
                srcRowDir = rotate(srcRowDir, rot);
                srcColDir = rotate(srcColDir, rot);
            }

            if (isFlipped(dstRowDir, dstColDir))
                srcColDir = rotate(srcColDir, 2);

            switch (srcRowDir)
            {
            case CardinalDirection::NORTH:
                if (srcColDir == CardinalDirection::EAST)
                    return Chorasmia::Index2DMode::COLUMNS_REVERSED_ORDER;
                else
                    return Chorasmia::Index2DMode::REVERSED_COLUMNS_REVERSED_ORDER;
            case CardinalDirection::EAST:
                if (srcColDir == CardinalDirection::NORTH)
                    return Chorasmia::Index2DMode::ROWS_REVERSED_ORDER;
                else
                    return Chorasmia::Index2DMode::ROWS;
            case CardinalDirection::SOUTH:
                if (srcColDir == CardinalDirection::EAST)
                    return Chorasmia::Index2DMode::COLUMNS;
                else
                    return Chorasmia::Index2DMode::REVERSED_COLUMNS;
            case CardinalDirection::WEST:
                if (srcColDir == CardinalDirection::NORTH)
                    return Chorasmia::Index2DMode::REVERSED_ROWS_REVERSED_ORDER;
                else
                    return Chorasmia::Index2DMode::REVERSED_ROWS;
            }
        }

        CardinalDirection getCardinalDirection(const Xyz::Vector2d& v)
        {
            constexpr auto PI = Xyz::Constants<double>::PI;
            auto a = Xyz::getCcwAngle<double>(v, {-1, 1});
            switch (int(floor(2 * a / PI)))
            {
            case 0:
                return CardinalDirection::NORTH;
            case 1:
                return CardinalDirection::EAST;
            case 2:
                return CardinalDirection::SOUTH;
            default:
                return CardinalDirection::WEST;
            }
        }
    }

    Chorasmia::Index2DMode
    getIndexModeForTopLeftOrigin(const GridView& grid)
    {
        auto rv = grid.rowAxis().direction;
        auto cv = grid.columnAxis().direction;
        auto rDir = getCardinalDirection(Xyz::makeVector2(rv[0], rv[1]));
        auto cDir = getCardinalDirection(Xyz::makeVector2(cv[0], cv[1]));
        return getIndex2DMode(rDir, cDir,
                              CardinalDirection::EAST,
                              CardinalDirection::SOUTH);
    }
}
