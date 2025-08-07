//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-11.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Xyz/CoordinateSystem.hpp>

#include "GridLib/IGrid.hpp"

namespace GridLib
{
    class PositionTransformer
    {
    public:
        Xyz::CoordinateSystem<double> cs;

        explicit PositionTransformer(const IGrid& grid);

        PositionTransformer(const Xyz::Matrix4D& matrix,
                            const Xyz::Vector2D& tie_point);

        [[nodiscard]]
        Xyz::Vector2D world_to_grid(const Xyz::Vector3D& pos) const;

        [[nodiscard]]
        Xyz::Vector3D grid_to_world(const Xyz::Vector2D& pos) const;

        [[nodiscard]]
        Xyz::Vector3D grid_to_world(const Xyz::Vector3D& pos) const;
    };
}
