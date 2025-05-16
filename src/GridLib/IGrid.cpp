//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/IGrid.hpp"
#include <Xyz/Interpolation.hpp>
#include "GridLib/Grid.hpp"
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    GridView IGrid::subgrid(size_t row, size_t column) const
    {
        return subgrid(row, column, SIZE_MAX, SIZE_MAX);
    }

    std::pair<float, float> get_min_max_elevation(const IGrid& grid)
    {
        const auto elevations = grid.values();
        const auto no_value = grid.model().unknown_elevation;
        auto min = FLT_MAX, max = FLT_TRUE_MIN;
        for (const auto row : elevations)
        {
            for (const auto value : row)
            {
                if (no_value && value == *no_value)
                    continue;
                if (value < min)
                    min = value;
                if (value > max)
                    max = value;
            }
        }

        if (min > max)
            return {};

        return {min, max};
    }

    bool is_planar(const IGrid& grid)
    {
        const auto& m = grid.model();
        return m.row_axis()[2] <= Xyz::Constants<double>::DEFAULT_MARGIN
               && m.column_axis()[2] <= Xyz::Constants<double>::DEFAULT_MARGIN;
    }

    namespace
    {
        double get_max_abs(double a, double b)
        {
            return std::abs(a) < std::abs(b) ? b : a;
        }
    }

    Xyz::RectangleD get_bounding_rect(const IGrid& grid)
    {
        if (!is_planar(grid))
            GRIDLIB_THROW("Can not calculate bounding rectangle for non-planar grid.");

        auto origin = grid_pos_to_model_pos(grid, {0, 0});
        auto rows = grid.row_count();
        auto cols = grid.col_count();
        const auto& m = grid.model();
        auto row_vec = double(cols - 1) * m.row_axis();
        auto col_vec = double(rows - 1) * m.column_axis();

        auto row_sign = get_max_abs(row_vec[0], row_vec[1]) > 0 ? 1 : -1;
        auto col_sign = get_max_abs(col_vec[0], col_vec[1]) > 0 ? 1 : -1;

        auto x_len = row_sign * get_length(row_vec);
        auto y_len = col_sign * get_length(col_vec);
        return {{origin[0], origin[1]}, {x_len, y_len}};
    }

    namespace
    {
        Xyz::Vector<size_t, 2> get_cell(const IGrid& grid,
                                        const Xyz::Vector2D& grid_pos)
        {
            if (grid_pos[0] < 0 || double(grid.row_count() - 1) < grid_pos[0])
                GRIDLIB_THROW("Row index out of bounds.");
            if (grid_pos[1] < 0 || double(grid.col_count() - 1) < grid_pos[1])
                GRIDLIB_THROW("Column index out of bounds.");
            const auto c = Xyz::vector_cast<size_t>(floor(grid_pos));
            return get_clamped(c, {0, 0}, {grid.row_count() - 2, grid.col_count() - 2});
        }

        Xyz::Vector4F get_grid_cell_values(const IGrid& grid,
                                           const Xyz::Vector<size_t, 2>& cell)
        {
            auto elevations = grid.values();
            return {
                elevations(cell[0], cell[1]),
                elevations(cell[0] + 1, cell[1]),
                elevations(cell[0], cell[1] + 1),
                elevations(cell[0] + 1, cell[1] + 1)
            };
        }

        bool has_unknown_elevation(const IGrid& grid,
                                   const Xyz::Vector4F& values)
        {
            if (!grid.model().unknown_elevation)
                return false;
            const auto unknown = *grid.model().unknown_elevation;
            return values[0] == unknown || values[1] == unknown
                   || values[2] == unknown || values[3] == unknown;
        }

        float get_edge_elevation(const IGrid& grid,
                                 Xyz::Vector2D& grid_pos,
                                 const Xyz::Vector<size_t, 2>& cell,
                                 const Xyz::Vector4F& values)
        {
            double ri;
            const auto rf = std::modf(grid_pos[0], &ri);
            double ci;
            const auto cf = std::modf(grid_pos[1], &ci);

            const auto r = unsigned(ri) - unsigned(cell[0]);
            const auto c = unsigned(ci) - unsigned(cell[1]);

            if (rf == 0 && cf == 0)
                return values[r + c * 2];

            const auto unknown = *grid.model().unknown_elevation;
            if (rf == 0)
            {
                if (values[r] != unknown && values[2 + r] != unknown)
                    return std::lerp(values[r], values[2 + r], float(cf));
            }
            else if (cf == 0)
            {
                if (values[2 * c] != unknown && values[2 * c + 1] != unknown)
                    return std::lerp(values[2 * c], values[2 * c + 1], float(rf));
            }

            return unknown;
        }
    }

    float get_elevation(const IGrid& grid, Xyz::Vector2D grid_pos)
    {
        auto cell = get_cell(grid, grid_pos);
        const auto cell_values = get_grid_cell_values(grid, cell);
        if (has_unknown_elevation(grid, cell_values))
            return get_edge_elevation(grid, grid_pos, cell, cell_values);

        const auto p1 = Xyz::vector_cast<double>(cell);
        const auto p2 = p1 + Xyz::Vector2D(1, 1);
        return bilinear(cell_values, grid_pos, p1, p2);
    }

    Xyz::Vector2D model_pos_to_grid_pos(const IGrid& grid,
                                        const Xyz::Vector3D& model_pos)
    {
        const auto& m = grid.model();
        const auto& row = m.row_axis();
        const auto& col = m.column_axis();
        const auto offset = model_pos - m.location();
        return grid.model_tie_point() + Xyz::Vector2D(
                   dot(offset, col) / dot(col, col),
                   dot(offset, row) / dot(row, row)
               );
    }

    Xyz::Vector3D grid_pos_to_model_pos(const IGrid& grid,
                                        const Xyz::Vector2D& grid_pos)
    {
        const auto offset = grid_pos - grid.model_tie_point();
        const auto& m = grid.model();
        return m.location()
               + offset[0] * m.column_axis()
               + offset[1] * m.row_axis();
    }
}
