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
    std::optional<PlanarCoords>
    get_planar_coords(double x, double y, int epsg)
    {
        if (25828 <= epsg && epsg <= 25838)
        {
            auto zone = epsg - 25828 + 28;
            return PlanarCoords{x, y, .zone=zone};
        }
        return {};
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
}
