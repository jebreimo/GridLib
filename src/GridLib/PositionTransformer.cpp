//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-11.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/PositionTransformer.hpp"

#include <Xyz/TransformationMatrix.hpp>
#include <Xyz/InvertMatrix.hpp>

namespace GridLib
{
    PositionTransformer::PositionTransformer(const IGrid& grid)
        : PositionTransformer(grid.spatial_info().matrix, grid.tie_point())
    {
    }

    PositionTransformer::PositionTransformer(const Xyz::Matrix4D& matrix,
        const Xyz::Vector2D& tie_point)
            : cs(matrix * Xyz::affine::translate3(make_vector3(-tie_point, 0.0)))
    {
    }

    Xyz::Vector2D PositionTransformer::world_to_grid(const Xyz::Vector3D& pos) const
    {
        return cs.to_cs_xy(pos);
    }

    Xyz::Vector3D PositionTransformer::grid_to_world(const Xyz::Vector2D& pos) const
    {
        return cs.from_cs_xy(pos);
    }

    Xyz::Vector3D PositionTransformer::grid_to_world(const Xyz::Vector3D& pos) const
    {
        return cs.from_cs(pos);
    }
}
