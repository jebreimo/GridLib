//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <Chorasmia/ArrayView2D.hpp>
#include <Xyz/Rectangle.hpp>

#include "GridMemberTypes.hpp"
#include "GridModel.hpp"

namespace GridLib
{
    class Grid;

    class GridView
    {
    public:
        GridView() noexcept;

        explicit GridView(const Grid& grid) noexcept;

        GridView(const Grid& grid,
                 const Chorasmia::ArrayView2D<float>& elevations,
                 const Xyz::Vector2D& model_tie_point = {}) noexcept;

        [[nodiscard]]
        size_t row_count() const;

        [[nodiscard]]
        size_t col_count() const;

        [[nodiscard]]
        Chorasmia::ArrayView2D<float> elevations() const;

        [[nodiscard]]
        const Xyz::Vector2D& model_tie_point() const;

        [[nodiscard]]
        const GridModel& model() const;

        [[nodiscard]]
        std::vector<SpatialTiePoint> spatial_tie_points() const;

        [[nodiscard]]
        const Grid* base_grid() const;

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column,
                         size_t n_rows, size_t n_cols) const;

    private:
        void assert_grid() const;

        const Grid* grid_ = nullptr;
        Chorasmia::ArrayView2D<float> elevations_;
        Xyz::Vector2D model_tie_point_;
    };

    [[nodiscard]]
    std::pair<float, float> get_min_max_elevation(const GridView& grid);

    [[nodiscard]]
    bool is_planar(const GridView& grid);

    [[nodiscard]]
    Xyz::RectangleD get_bounding_rect(const GridView& grid);

    /**
     * @brief Get the elevation at a given grid position.
     *
     * @param grid The grid to get the elevation from.
     * @param grid_pos The position, in row-major order, in the grid to get
     *      the elevation from.
     * @return The elevation at the given position.
     */
    [[nodiscard]]
    float get_elevation(const GridView& grid, Xyz::Vector2D grid_pos);

    /**
     * @brief Get the grid position at a given model position.
     *
     * @param grid The grid to get the position for.
     * @param model_pos The position in model coordinates. The order of the
     *      coordinates are easting, northing, elevation.
     * @return The elevation at the given position.
     */
    [[nodiscard]]
    Xyz::Vector2D model_pos_to_grid_pos(const GridView& grid,
                                        const Xyz::Vector3D& model_pos);

    [[nodiscard]]
    Xyz::Vector3D grid_pos_to_model_pos(const GridView& grid,
                                        const Xyz::Vector2D& grid_pos);
}
