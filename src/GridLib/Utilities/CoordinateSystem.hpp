//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-02-15.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "GridLib/GridMemberTypes.hpp"

namespace GridLib
{
    std::optional<PlanarCoords>
    get_planar_coords(double x, double y, int epsg);

    std::optional<SphericalCoords>
    get_spherical_coords(double x, double y, int epsg);

    Unit epsg_to_unit(int epsg);
}
