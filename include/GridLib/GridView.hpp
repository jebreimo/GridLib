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
        size_t row_count() const override;

        [[nodiscard]]
        size_t col_count() const override;

        [[nodiscard]]
        const Xyz::Vector2D& model_tie_point() const override;

        [[nodiscard]]
        const SpatialData& spatial_data() const override;

        [[nodiscard]]
        std::vector<SpatialTiePoint> spatial_tie_points() const override;

        [[nodiscard]]
        Chorasmia::ArrayView2D<float> values() const override;

        [[nodiscard]]
        const Grid* base_grid() const;

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column,
                         size_t n_rows, size_t n_cols) const override;

    private:
        void assert_grid() const;

        const Grid* grid_ = nullptr;
        Chorasmia::ArrayView2D<float> elevations_;
        Xyz::Vector2D model_tie_point_;
    };
}
