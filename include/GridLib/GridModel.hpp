//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
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

        [[nodiscard]]
        Xyz::Vector3D location() const
        {
            return {matrix[{0, 3}], matrix[{1, 3}], matrix[{2, 3}]};
        }

        void set_location(const Xyz::Vector3D& v)
        {
            matrix[{0, 3}] = v[0];
            matrix[{1, 3}] = v[1];
            matrix[{2, 3}] = v[2];
        }

        [[nodiscard]]
        Xyz::Vector3D column_axis() const
        {
            return {matrix[{0, 0}], matrix[{1, 0}], matrix[{2, 0}]};
        }

        void set_column_axis(const Xyz::Vector3D& v)
        {
            matrix[{0, 0}] = v[0];
            matrix[{1, 0}] = v[1];
            matrix[{2, 0}] = v[2];
        }

        [[nodiscard]]
        Xyz::Vector3D row_axis() const
        {
            return {matrix[{0, 1}], matrix[{1, 1}], matrix[{2, 1}]};
        }

        void set_row_axis(const Xyz::Vector3D& v)
        {
            matrix[{0, 1}] = v[0];
            matrix[{1, 1}] = v[1];
            matrix[{2, 1}] = v[2];
        }

        [[nodiscard]]
        Xyz::Vector3D vertical_axis() const
        {
            return {matrix[{0, 2}], matrix[{1, 2}], matrix[{2, 2}]};
        }

        void set_vertical_axis(const Xyz::Vector3D& v)
        {
            matrix[{0, 2}] = v[0];
            matrix[{1, 2}] = v[1];
            matrix[{2, 2}] = v[2];
        }
    };

    bool operator==(const GridModel& lhs, const GridModel& rhs);

    bool operator!=(const GridModel& lhs, const GridModel& rhs);
}
