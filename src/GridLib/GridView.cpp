//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Xyz/Interpolation.hpp>
#include "GridLib/GridView.hpp"
#include "GridLib/Grid.hpp"
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    GridView::GridView() noexcept = default;

    GridView::GridView(const Grid& grid) noexcept
        : GridView(grid,
                   grid.elevations(),
                   grid.coordinates())
    {
    }

    GridView::GridView(const Grid& grid,
                       const Chorasmia::ArrayView2D<float>& elevations,
                       const Coordinates& coords) noexcept
        : grid_(&grid),
          elevations_(elevations),
          coordinates_(coords)
    {
    }

    size_t GridView::row_count() const
    {
        return elevations_.row_count();
    }

    size_t GridView::col_count() const
    {
        return elevations_.col_count();
    }

    Chorasmia::ArrayView2D<float> GridView::elevations() const
    {
        return elevations_;
    }

    std::optional<float> GridView::unknown_elevation() const
    {
        assert_grid();
        return grid_->unknown_elevation();
    }

    const Xyz::Vector3D& GridView::row_axis() const
    {
        assert_grid();
        return grid_->row_axis();
    }

    const Xyz::Vector3D& GridView::col_axis() const
    {
        assert_grid();
        return grid_->column_axis();
    }

    const Xyz::Vector3D& GridView::vertical_axis() const
    {
        assert_grid();
        return grid_->vertical_axis();
    }

    Unit GridView::horizontal_unit() const
    {
        return grid_->horizontal_unit();
    }

    Unit GridView::vertical_unit() const
    {
        return grid_->vertical_unit();
    }

    const Coordinates& GridView::coordinates() const
    {
        return coordinates_;
    }

    const CoordinateReferenceSystem& GridView::reference_system() const
    {
        assert_grid();
        return grid_->reference_system();
    }

    const Grid* GridView::base_grid() const
    {
        return grid_;
    }

    GridView GridView::subgrid(size_t row, size_t column,
                               size_t n_rows, size_t n_cols) const
    {
        assert_grid();
        auto coords = coordinates();
        if (row != 0 || column != 0)
            coords.grid -= {float(row), float(column)};
        return {
            *grid_,
            elevations_.subarray(row, column, n_rows, n_cols),
            coords
        };
    }

    void GridView::assert_grid() const
    {
        if (!grid_)
            GRIDLIB_THROW("grid is NULL");
    }

    std::pair<float, float> get_min_max_elevation(const GridView& grid)
    {
        const auto elevations = grid.elevations();
        const auto no_value = grid.unknown_elevation();
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

    bool is_planar(const GridView& grid)
    {
        const auto& row_vec = grid.col_axis();
        const auto& col_vec = grid.row_axis();

        return row_vec[2] <= Xyz::Constants<double>::DEFAULT_MARGIN
               && col_vec[2] <= Xyz::Constants<double>::DEFAULT_MARGIN;
    }

    namespace
    {
        double get_max_abs(double a, double b)
        {
            return std::abs(a) < std::abs(b) ? b : a;
        }
    }

    Xyz::RectangleD get_bounding_rect(const GridView& grid)
    {
        if (!is_planar(grid))
            GRIDLIB_THROW("Can not calculate bounding rectangle for non-planar grid.");

        auto origin = grid_pos_to_model_pos(grid, {0, 0});
        auto [rows, cols] = grid.elevations().dimensions();
        auto row_vec = double(cols - 1) * grid.row_axis();
        auto col_vec = double(rows - 1) * grid.col_axis();

        auto row_sign = get_max_abs(row_vec[0], row_vec[1]) > 0 ? 1 : -1;
        auto col_sign = get_max_abs(col_vec[0], col_vec[1]) > 0 ? 1 : -1;

        auto x_len = row_sign * get_length(row_vec);
        auto y_len = col_sign * get_length(col_vec);
        return {{origin[0], origin[1]}, {x_len, y_len}};
    }

    namespace
    {
        Xyz::Vector<size_t, 2> get_cell(const GridView& grid,
                                        const Xyz::Vector2D& grid_pos)
        {
            if (grid_pos[0] < 0 || double(grid.row_count() - 1) < grid_pos[0])
                GRIDLIB_THROW("Row index out of bounds.");
            if (grid_pos[1] < 0 || double(grid.col_count() - 1) < grid_pos[1])
                GRIDLIB_THROW("Column index out of bounds.");
            const auto c = Xyz::vector_cast<size_t>(floor(grid_pos));
            return get_clamped(c, {0, 0}, {grid.row_count() - 2, grid.col_count() - 2});
        }

        Xyz::Vector4F get_grid_cell_values(const GridView& grid,
                                           const Xyz::Vector<size_t, 2>& cell)
        {
            auto elevations = grid.elevations();
            return {
                elevations(cell[0], cell[1]),
                elevations(cell[0], cell[1] + 1),
                elevations(cell[0] + 1, cell[1]),
                elevations(cell[0] + 1, cell[1] + 1)
            };
        }

        bool has_unknown_elevation(const GridView& grid,
                                   const Xyz::Vector4F& values)
        {
            if (!grid.unknown_elevation())
                return false;
            const auto unknown = *grid.unknown_elevation();
            return values[0] == unknown || values[1] == unknown
                   || values[2] == unknown || values[3] == unknown;
        }

        float get_edge_elevation(const GridView& grid,
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
                return values[r * 2 + c];

            const auto unknown = *grid.unknown_elevation();
            if (rf == 0)
            {
                if (values[r * 2] != unknown && values[r * 2 + 1] != unknown)
                    return std::lerp(values[r * 2], values[r * 2 + 1], float(cf));
            }
            else if (cf == 0)
            {
                if (values[c] != unknown && values[2 + c] != unknown)
                    return std::lerp(values[c], values[2 + c], float(rf));
            }

            return unknown;
        }
    }

    float get_elevation(const GridView& grid, Xyz::Vector2D grid_pos)
    {
        auto cell = get_cell(grid, grid_pos);
        const auto cell_values = get_grid_cell_values(grid, cell);
        if (has_unknown_elevation(grid, cell_values))
            return get_edge_elevation(grid, grid_pos, cell, cell_values);

        // Xyz::bilinear assumes column-major order, so we need to swap the
        // coordinates.
        std::swap(grid_pos[0], grid_pos[1]);
        std::swap(cell[0], cell[1]);

        const auto p1 = Xyz::vector_cast<float>(cell);
        const auto p2 = p1 + Xyz::Vector2F(1, 1);
        return bilinear(cell_values, p1, p2, Xyz::vector_cast<float>(grid_pos));
    }

    Xyz::Vector2D model_pos_to_grid_pos(const GridView& grid,
                                        const Xyz::Vector3D& model_pos)
    {
        const auto& row = grid.row_axis();
        const auto& col = grid.col_axis();
        const auto offset = model_pos - grid.coordinates().model;
        return grid.coordinates().grid + Xyz::Vector2D(
                   dot(offset, col) / dot(col, col),
                   dot(offset, row) / dot(row, row)
               );
    }

    Xyz::Vector3D
    grid_pos_to_model_pos(const GridView& grid, const Xyz::Vector2D& grid_pos)
    {
        const auto offset = grid_pos - grid.coordinates().grid;
        return grid.coordinates().model
               + offset[0] * grid.col_axis()
               + offset[1] * grid.row_axis();
    }
}
