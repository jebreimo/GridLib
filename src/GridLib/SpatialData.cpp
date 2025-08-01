//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/SpatialData.hpp"

namespace GridLib
{
    Xyz::Vector3D SpatialData::location() const
    {
        return {matrix[{0, 3}], matrix[{1, 3}], matrix[{2, 3}]};
    }

    void SpatialData::set_location(const Xyz::Vector3D& v)
    {
        matrix[{0, 3}] = v[0];
        matrix[{1, 3}] = v[1];
        matrix[{2, 3}] = v[2];
    }

    Xyz::Vector3D SpatialData::column_axis() const
    {
        return {matrix[{0, 0}], matrix[{1, 0}], matrix[{2, 0}]};
    }

    void SpatialData::set_column_axis(const Xyz::Vector3D& v)
    {
        matrix[{0, 0}] = v[0];
        matrix[{1, 0}] = v[1];
        matrix[{2, 0}] = v[2];
    }

    Xyz::Vector3D SpatialData::row_axis() const
    {
        return {matrix[{0, 1}], matrix[{1, 1}], matrix[{2, 1}]};
    }

    void SpatialData::set_row_axis(const Xyz::Vector3D& v)
    {
        matrix[{0, 1}] = v[0];
        matrix[{1, 1}] = v[1];
        matrix[{2, 1}] = v[2];
    }

    Xyz::Vector3D SpatialData::vertical_axis() const
    {
        return {matrix[{0, 2}], matrix[{1, 2}], matrix[{2, 2}]};
    }

    void SpatialData::set_vertical_axis(const Xyz::Vector3D& v)
    {
        matrix[{0, 2}] = v[0];
        matrix[{1, 2}] = v[1];
        matrix[{2, 2}] = v[2];
    }

    bool operator==(const SpatialData& lhs, const SpatialData& rhs)
    {
        return lhs.matrix == rhs.matrix
               && lhs.horizontal_unit == rhs.horizontal_unit
               && lhs.vertical_unit == rhs.vertical_unit
               && lhs.crs == rhs.crs
               && lhs.unknown_elevation == rhs.unknown_elevation
               && lhs.information == rhs.information;
    }

    bool operator!=(const SpatialData& lhs, const SpatialData& rhs)
    {
        return !(lhs == rhs);
    }
}
