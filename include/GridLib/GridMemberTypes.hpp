//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the BSD License.
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
        FEET = 1,
        METERS = 2,
        ARC_SECONDS = 3
    };

    std::string_view to_string(Unit unit);

    std::optional<Unit> parse_unit(std::string_view str);

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
        Unit unit;
    };

    constexpr bool operator==(const Axis& a, const Axis& b)
    {
        return a.direction == b.direction
               && a.unit == b.unit;
    }

    struct SphericalCoords
    {
        double latitude = 0;
        double longitude = 0;
    };

    constexpr bool operator==(const SphericalCoords& a, const SphericalCoords& b)
    {
        return a.latitude == b.latitude && a.longitude == b.longitude;
    }

    struct PlanarCoords
    {
        double easting = 0;
        double northing = 0;
        int zone = 0;
    };

    constexpr bool operator==(const PlanarCoords& a, const PlanarCoords& b)
    {
        return a.easting == b.easting
               && a.northing == b.northing
               && a.zone == b.zone;
    }

    struct ReferenceSystem
    {
        int horizontal = 0;
        int vertical = 0;
    };

    constexpr bool operator==(const ReferenceSystem& a, const ReferenceSystem& b)
    {
        return a.horizontal == b.horizontal && a.vertical == b.vertical;
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
