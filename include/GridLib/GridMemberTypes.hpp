//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <Xyz/Vector.hpp>
#include "Unit.hpp"

namespace GridLib
{
    struct Coordinates
    {
        /**
         * @brief The model coordinates in either planar or graphical
         *      coordinate system.
         *
         * The coordinate units are the same as the axes.
         */
        Xyz::Vector3D model;

        /**
         * @brief The grid position, row and column, that correspond to the
         *      planar and/or geographical coordinates.
         */
        Xyz::Vector2D grid;

        /**
         * @brief The spherical model coordinates in the coordinate system.
         *
         * The coordinate order is easting, northing.
         */
        std::optional<Xyz::Vector2D> planar;

        /**
         * @brief The spherical model coordinates in the coordinate system.
         *
         * The coordinate order is latitude, longitude.
         */
        std::optional<Xyz::Vector2D> geographic;
    };

    constexpr bool operator==(const Coordinates& a, const Coordinates& b)
    {
        return a.model == b.model
               && a.geographic == b.geographic
               && a.grid == b.grid;
    }

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

    struct CoordinateReferenceSystem
    {
        int projected = 0;
        int vertical = 0;
        int geographic = 0;
        int zone = 0;

        explicit operator bool() const
        {
            return projected != 0 || vertical != 0
                   || geographic != 0 || zone != 0;
        }
    };

    constexpr bool operator==(const CoordinateReferenceSystem& a,
                              const CoordinateReferenceSystem& b)
    {
        return a.projected == b.projected
               && a.vertical == b.vertical
               && a.geographic == b.geographic
               && a.zone == b.zone;
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
