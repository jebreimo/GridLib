//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-03-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Rasterize.hpp"

namespace GridLib
{
    namespace
    {
        union Color
        {
            struct
            {
                uint8_t a;
                uint8_t b;
                uint8_t g;
                uint8_t r;
            };
            uint32_t rgba;
        };

        void addRange(Chorasmia::IntervalMap<double, uint32_t>& map,
                      double fromHeight, double toHeight,
                      Color fromColor, Color toColor,
                      unsigned steps)
        {
            uint32_t base[4] = {fromColor.r, fromColor.g, fromColor.b, fromColor.a};
            uint32_t delta[4] = {uint32_t(toColor.r - fromColor.r),
                                 uint32_t(toColor.g - fromColor.g),
                                 uint32_t(toColor.b - fromColor.b),
                                 uint32_t(toColor.a - fromColor.a)};
            for (uint32_t i = 0; i < steps; ++i)
            {
                Color c{.a = uint8_t(base[3] + (delta[3] * i) / (steps - 1)),
                    .b = uint8_t(base[2] + (delta[2] * i) / (steps - 1)),
                    .g = uint8_t(base[1] + (delta[1] * i) / (steps - 1)),
                    .r = uint8_t(base[0] + (delta[0] * i) / (steps - 1))};
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
        Chorasmia::IntervalMap<double, uint32_t> map(0x5A8EFFFFu);
        map.insert(seaLevelMax, 0xE9F2FCFFu);
        addRange(map, seaLevelMin, seaLevelMax, Color{.rgba = 0x5A8EFFFFu},
                 Color{.rgba = 0xD1EAFAFFu}, 10);
        addRange(map, seaLevelMax, groundLevel1Max, Color{.rgba = 0x7C9074FFu},
                 Color{.rgba = 0xD7E8D0FFu}, 10);
        addRange(map, groundLevel1Max, groundLevel2Max, Color{.rgba = 0xE3DFADFFu},
                 Color{.rgba = 0x8C886FFFu}, 10);
        addRange(map, groundLevel2Max, groundLevel3Max, Color{.rgba = 0x908069FFu},
                 Color{.rgba = 0xD1B995FFu}, 10);
        addRange(map, groundLevel3Max, groundLevel4Max, Color{.rgba = 0xC1A995FFu},
                 Color{.rgba = 0xDCDCE3FFu}, 10);
        addRange(map, groundLevel4Max, groundLevel5Max, Color{.rgba = 0xA6C8ECFFu},
                 Color{.rgba = 0xE9F2FCFFu}, 10);

        // Extend the sea a little bit:
        map.insert(seaLevelMax, seaLevelMax + 0.25, 0xD1EAFAFFu);

        return map;
    }

    Chorasmia::IntervalMap<double, uint32_t> makeEarthGradient2500()
    {
        return makeMapGradient(2500, 0, 500, 1000, 1500, 2000, 2500);
    }

    Chorasmia::IntervalMap<double, uint32_t> makeEarthGradient9000()
    {
        return makeMapGradient(2500, 0, 1000, 2000, 3000, 5000, 9000);
    }
}
