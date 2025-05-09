//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-02-15.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "GridLib/GridMemberTypes.hpp"
#include "GridLib/Unit.hpp"

namespace GridLib
{
    std::optional<Xyz::Vector2D>
    get_geographic_coords(double x, double y, int epsg);

    Unit epsg_unit_to_unit(int epsg);

    Unit epsg_crs_to_horizontal_unit(int epsg);

    Unit epsg_crs_to_vertical_unit(int epsg);
}
