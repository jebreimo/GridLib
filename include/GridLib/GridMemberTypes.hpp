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

namespace GridLib
{
    enum class Unit
    {
        UNDEFINED = 0,
        FEET = 1,
        METERS = 2,
        ARC_SECONDS = 3
    };

    std::string_view toString(Unit unit);

    std::optional<Unit> parseUnit(std::string_view str);

    struct Axis
    {
        double resolution = {};
        Unit unit = {};
    };

    constexpr bool operator==(const Axis& a, const Axis& b)
    {
        return a.resolution == b.resolution && a.unit == b.unit;
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
}
