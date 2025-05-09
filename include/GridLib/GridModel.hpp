//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <vector>
#include <Xyz/Matrix.hpp>
#include "GridLib/GridMemberTypes.hpp"
#include "GridLib/Unit.hpp"

namespace GridLib
{
    struct GridModel
    {
        Xyz::Matrix4D matrix = Xyz::Matrix4D::identity();

        Unit horizontal_unit = Unit::UNDEFINED;

        Unit vertical_unit = Unit::UNDEFINED;

        CoordinateReferenceSystem reference_system;

        std::optional<float> unknown_elevation;

        std::vector<std::pair<std::string, std::string>> information;

        [[nodiscard]] Xyz::Vector3D location() const;

        void set_location(const Xyz::Vector3D& v);

        [[nodiscard]] Xyz::Vector3D column_axis() const;

        void set_column_axis(const Xyz::Vector3D& v);

        [[nodiscard]] Xyz::Vector3D row_axis() const;

        void set_row_axis(const Xyz::Vector3D& v);

        [[nodiscard]] Xyz::Vector3D vertical_axis() const;

        void set_vertical_axis(const Xyz::Vector3D& v);
    };

    bool operator==(const GridModel& lhs, const GridModel& rhs);

    bool operator!=(const GridModel& lhs, const GridModel& rhs);
}
