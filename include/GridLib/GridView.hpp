//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <Chorasmia/ArrayView2D.hpp>
#include <Xyz/Rectangle.hpp>

#include "GridMemberTypes.hpp"

namespace GridLib
{
    class Grid;

    class GridView
    {
    public:
        GridView() noexcept;

        explicit GridView(const Grid& grid) noexcept;

        GridView(const Grid& grid,
                 Chorasmia::ArrayView2D<float> elevations,
                 std::optional<SphericalCoords> spherical_coords,
                 std::optional<PlanarCoords> planar_coords) noexcept;

        [[nodiscard]]
        size_t row_count() const;

        [[nodiscard]]
        size_t col_count() const;

        [[nodiscard]]
        Chorasmia::ArrayView2D<float> elevations() const;

        [[nodiscard]]
        std::optional<float> unknown_elevation() const;

        [[nodiscard]]
        const Axis& row_axis() const;

        [[nodiscard]]
        const Axis& col_axis() const;

        [[nodiscard]]
        const Axis& vertical_axis() const;

        [[nodiscard]]
        const std::optional<SphericalCoords>& spherical_coords() const;

        [[nodiscard]]
        const std::optional<PlanarCoords>& planar_coords() const;

        [[nodiscard]]
        const std::optional<ReferenceSystem>& reference_system() const;

        [[nodiscard]]
        const Grid* base_grid() const;

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column,
                         size_t n_rows, size_t n_cols) const;
    private:
        void assert_grid() const;

        const Grid* grid_ = nullptr;
        Chorasmia::ArrayView2D<float> elevations_;
        std::optional<SphericalCoords> spherical_coords_;
        std::optional<PlanarCoords> planar_coords_;
    };

    [[nodiscard]]
    std::pair<float, float> get_min_max_elevation(const GridView& grid);

    [[nodiscard]]
    bool is_planar(const GridView& grid);

    [[nodiscard]]
    Xyz::RectangleD get_bounding_rect(const GridView& grid);
}
