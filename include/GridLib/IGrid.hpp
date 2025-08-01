//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-04-24.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Chorasmia/ArrayView2D.hpp>
#include <Xyz/Pgram3.hpp>
#include "GridLib/SpatialData.hpp"

namespace GridLib
{
    class GridView;

    class IGrid
    {
    public:
        virtual ~IGrid() = default;

        [[nodiscard]]
        virtual size_t row_count() const = 0;

        [[nodiscard]]
        virtual size_t col_count() const = 0;

        [[nodiscard]]
        virtual const Xyz::Vector2D& tie_point() const = 0;

        [[nodiscard]]
        virtual const SpatialData& spatial_data() const = 0;

        [[nodiscard]]
        virtual std::vector<SpatialTiePoint> spatial_tie_points() const = 0;

        [[nodiscard]]
        virtual Chorasmia::ArrayView2D<float> values() const = 0;

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column) const;

        [[nodiscard]]
        virtual GridView subgrid(size_t row, size_t column,
                                 size_t n_rows, size_t n_cols) const = 0;
    };

    [[nodiscard]]
    std::pair<float, float> get_min_max_elevation(const IGrid& grid);

    [[nodiscard]]
    bool is_elevation_grid(const IGrid& grid);

    [[nodiscard]]
    Xyz::Pgram3D get_bounds(const IGrid& grid);
}
