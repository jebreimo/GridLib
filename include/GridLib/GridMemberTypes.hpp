//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <string_view>
#include <Xyz/Vector.hpp>

namespace GridLib
{
    enum class Unit
    {
        UNDEFINED = 0,
        METER = 1,
        FOOT = 2,
        US_SURVEY_FOOT = 3,
        DEGREE = 4,
        ARC_SECOND = 5
    };

    std::string_view to_string(Unit unit);

    std::optional<Unit> parse_unit(std::string_view str);

    double to_meters(Unit unit);

    struct Axis
    {
        /**
         * @brief The axis' direction and point distance in a metric easting,
         *  northing, height (i.e. x, y, z) coordinate system.
         *
         * The length of this vector is the distance in meters between points
         * along the axis. If the distance between points is 10 meters, the
         * length of the vector should be 10, if it is 10 feet it should be
         * 10 * 0.3048 = 3.048.
         */
        Xyz::Vector3D direction;
        Unit unit = Unit::UNDEFINED;
    };

    constexpr bool operator==(const Axis& a, const Axis& b)
    {
        return a.direction == b.direction
               && a.unit == b.unit;
    }

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
