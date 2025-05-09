//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <variant>
#include <Xyz/Vector.hpp>

namespace GridLib
{
    struct ProjectedCrs
    {
        int projection = 0;
        int vertical = 0;

        explicit operator bool() const
        {
            return projection != 0 || vertical != 0;
        }
    };

    inline bool operator==(const ProjectedCrs& a, const ProjectedCrs& b)
    {
        return a.projection == b.projection
                && a.vertical == b.vertical;
    }

    struct GeographicCrs
    {
        int geographic = 0;
        int vertical = 0;

        explicit operator bool() const
        {
            return geographic != 0 || vertical != 0;
        }
    };

    inline bool operator==(const GeographicCrs& a, const GeographicCrs& b)
    {
        return a.geographic == b.geographic
               && a.vertical == b.vertical;
    }

    using Crs = std::variant<std::monostate, ProjectedCrs, GeographicCrs>;

    struct SpatialTiePoint
    {
        Xyz::Vector2D grid_point;
        Xyz::Vector3D location;
        Crs crs;
    };

    inline bool operator==(const SpatialTiePoint& a, const SpatialTiePoint& b)
    {
        return a.grid_point == b.grid_point
               && a.location == b.location
               && a.crs == b.crs;
    }

    enum class RotationDir
    {
        CLOCKWISE,
        COUNTERCLOCKWISE
    };

    constexpr double NORTH = 0.0;
    constexpr double WEST = 1.5707963267948966;
    constexpr double SOUTH = 3.141592653589793;
    constexpr double EAST = 4.71238898038469;
}
