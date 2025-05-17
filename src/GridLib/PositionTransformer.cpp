//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-11.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/PositionTransformer.hpp"

#include <Xyz/MatrixTransformations.hpp>
#include <Xyz/InvertMatrix.hpp>

namespace GridLib
{
    PositionTransformer::PositionTransformer(const IGrid& grid)
        : grid(grid),
          transform_(grid.model().matrix * translate4(make_vector3(-grid.model_tie_point(), 0.0))),
          inverse_transform_(Xyz::invert(transform_))
    {
    }

    Xyz::Vector2D PositionTransformer::model_to_grid(const Xyz::Vector3D& pos) const
    {
        auto p = inverse_transform_ * Xyz::make_vector4(pos, 1.0);
        return {p[0], p[1]};
    }

    Xyz::Vector3D PositionTransformer::grid_to_model(const Xyz::Vector2D& pos) const
    {
        auto p = transform_ * Xyz::Vector4D(pos[0], pos[1], 0.0, 1.0);
        return {p[0], p[1], p[2]};
    }

    Xyz::Vector3D PositionTransformer::grid_to_model(const Xyz::Vector3D& pos) const
    {
        auto p = transform_ * Xyz::make_vector4(pos, 1.0);
        return {p[0], p[1], p[2]};
    }
}
