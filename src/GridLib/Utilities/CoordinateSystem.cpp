//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-02-15.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "CoordinateSystem.hpp"

#include "Utm.hpp"

namespace GridLib
{
    int get_crs_zone(int epsg)
    {
        if (25828 <= epsg && epsg <= 25838)
            return epsg - 25828 + 28;
        return 0;
    }

    std::optional<PlanarCoords>
    get_planar_coords(double x, double y, int epsg)
    {
        return PlanarCoords{x, y, 0, get_crs_zone(epsg)};
    }

    std::optional<SphericalCoords>
    get_spherical_coords(double x, double y, int epsg)
    {
        if (25828 <= epsg && epsg <= 25838)
        {
            auto zone = epsg - 25828 + 28;
            return utm_to_geo(x, y, zone);
        }
        return {};
    }

    Unit epsg_to_unit(int epsg)
    {
        switch (epsg)
        {
        case 9001:
            return Unit::METER;
        case 9002:
            return Unit::FOOT;
        case 9003:
            return Unit::US_SURVEY_FOOT;
        default:
            return Unit::UNDEFINED;
        }
    }
}
