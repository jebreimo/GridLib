//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-12.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "PositionTransformer.hpp"

namespace GridLib
{
    class GridInterpolator
    {
    public:
        PositionTransformer transformer;
        const IGrid* grid = nullptr;

        explicit GridInterpolator(const IGrid& grid);

        [[nodiscard]]
        std::optional<double>
        raw_value_at_grid_pos(const Xyz::Vector2D& grid_pos) const;

        [[nodiscard]]
        std::optional<double>
        raw_value_at_model_pos(const Xyz::Vector3D& model_pos) const;

        [[nodiscard]]
        std::optional<Xyz::Vector3D>
        at_grid_pos(const Xyz::Vector2D& grid_pos) const;

        [[nodiscard]]
        std::optional<Xyz::Vector3D>
        at_model_pos(const Xyz::Vector3D& model_pos) const;
    };
} // GridLib
