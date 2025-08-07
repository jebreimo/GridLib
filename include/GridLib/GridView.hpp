//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IGrid.hpp"

namespace GridLib
{
    class Grid;

    class GridView : public IGrid
    {
    public:
        GridView() noexcept;

        explicit GridView(const Grid& grid) noexcept;

        GridView(const Grid& grid,
                 const Chorasmia::ArrayView2D<float>& elevations,
                 const Xyz::Vector2D& model_tie_point = {}) noexcept;

        [[nodiscard]]
        Size size() const override;

        [[nodiscard]]
        const Xyz::Vector2D& tie_point() const override;

        [[nodiscard]]
        const SpatialInfo& spatial_info() const override;

        [[nodiscard]]
        std::vector<SpatialTiePoint> spatial_tie_points() const override;

        [[nodiscard]]
        Chorasmia::ArrayView2D<float> values() const override;

        [[nodiscard]]
        const Grid* base_grid() const;

        [[nodiscard]]
        GridView subgrid(const Index& index, const Size& size) const override;

    private:
        void assert_grid() const;

        const Grid* grid_ = nullptr;
        Chorasmia::ArrayView2D<float> values_;
        Xyz::Vector2D model_tie_point_;
    };
}
