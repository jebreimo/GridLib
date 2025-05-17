//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-11.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Xyz/Vector.hpp>

#include "GridLib/IGrid.hpp"

namespace GridLib
{
    class PositionTransformer
    {
    public:
        explicit PositionTransformer(const IGrid& grid);

        const IGrid& grid;

        [[nodiscard]]
        Xyz::Vector2D model_to_grid(const Xyz::Vector3D& pos) const;

        [[nodiscard]]
        Xyz::Vector3D grid_to_model(const Xyz::Vector2D& pos) const;

        [[nodiscard]]
        Xyz::Vector3D grid_to_model(const Xyz::Vector3D& pos) const;
    private:
        const Xyz::Matrix4D transform_;
        const Xyz::Matrix4D inverse_transform_;
    };
}
