//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-04-24.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Chorasmia/ArrayView2D.hpp>
#include <Xyz/Rectangle.hpp>
#include "GridLib/GridModel.hpp"

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
        virtual const Xyz::Vector2D& model_tie_point() const = 0;

        [[nodiscard]]
        virtual const GridModel& model() const = 0;

        [[nodiscard]]
        virtual std::vector<SpatialTiePoint> spatial_tie_points() const = 0;

        [[nodiscard]]
        virtual Chorasmia::ArrayView2D<float> elevations() const = 0;

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column) const;

        [[nodiscard]]
        virtual GridView subgrid(size_t row, size_t column,
                                 size_t n_rows, size_t n_cols) const = 0;
    };

    [[nodiscard]]
    std::pair<float, float> get_min_max_elevation(const IGrid& grid);

    [[nodiscard]]
    bool is_planar(const IGrid& grid);

    [[nodiscard]]
    Xyz::RectangleD get_bounding_rect(const IGrid& grid);

    /**
     * @brief Get the elevation at a given grid position.
     *
     * @param grid The grid to get the elevation from.
     * @param grid_pos The position, in row-major order, in the grid to get
     *      the elevation from.
     * @return The elevation at the given position.
     */
    [[nodiscard]]
    float get_elevation(const IGrid& grid, Xyz::Vector2D grid_pos);

    /**
     * @brief Get the grid position at a given model position.
     *
     * @param grid The grid to get the position for.
     * @param model_pos The position in model coordinates. The order of the
     *      coordinates are easting, northing, elevation.
     * @return The elevation at the given position.
     */
    [[nodiscard]]
    Xyz::Vector2D model_pos_to_grid_pos(const IGrid& grid,
                                        const Xyz::Vector3D& model_pos);

    [[nodiscard]]
    Xyz::Vector3D grid_pos_to_model_pos(const IGrid& grid,
                                        const Xyz::Vector2D& grid_pos);
}
