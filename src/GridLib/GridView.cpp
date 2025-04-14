//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the BSD License.
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
                   grid.spherical_coords(),
                   grid.planar_coords())
    {
    }

    GridView::GridView(const Grid& grid,
                       Chorasmia::ArrayView2D<float> elevations,
                       std::optional<SphericalCoords> spherical_coords,
                       std::optional<PlanarCoords> planar_coords) noexcept
        : grid_(&grid),
          elevations_(elevations),
          spherical_coords_(spherical_coords),
          planar_coords_(planar_coords)
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

    const Axis& GridView::row_axis() const
    {
        assert_grid();
        return grid_->row_axis();
    }

    const Axis& GridView::col_axis() const
    {
        assert_grid();
        return grid_->column_axis();
    }

    const Axis& GridView::vertical_axis() const
    {
        assert_grid();
        return grid_->vertical_axis();
    }

    const std::optional<SphericalCoords>& GridView::spherical_coords() const
    {
        return spherical_coords_;
    }

    const std::optional<PlanarCoords>& GridView::planar_coords() const
    {
        return planar_coords_;
    }

    const std::optional<ReferenceSystem>& GridView::reference_system() const
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
        auto planar_coords = planar_coords_;
        if (planar_coords && (row != 0 || column != 0))
        {
            auto offset = double(row) * col_axis().direction
                          + double(column) * row_axis().direction;
            planar_coords->easting += offset[0];
            planar_coords->northing += offset[1];
        }
        auto spherical_coords = row == 0 && column == 0
                                    ? spherical_coords_
                                    : std::optional<SphericalCoords>();
        return {
            *grid_,
            elevations_.subarray(row, column, n_rows, n_cols),
            spherical_coords, planar_coords
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
        auto min = DBL_MAX, max = DBL_TRUE_MIN;
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
        const auto row_vec = grid.col_axis().direction;
        const auto col_vec = grid.row_axis().direction;

        constexpr Xyz::Vector3D up(0, 0, 1);
        return dot(row_vec, up) <= Xyz::Constants<double>::DEFAULT_MARGIN
               && dot(col_vec, up) <= Xyz::Constants<double>::DEFAULT_MARGIN;
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
        auto pos = grid.planar_coords();
        auto origin = pos
                          ? Xyz::Vector3D(pos->easting, pos->northing, pos->elevation)
                          : Xyz::Vector3D();
        auto [rows, cols] = grid.elevations().dimensions();
        auto row_vec = double(cols) * grid.row_axis().direction;
        auto col_vec = double(rows) * grid.col_axis().direction;

        if (!is_planar(grid))
        {
            GRIDLIB_THROW("Can not calculate bounding rectangle for non-planar grid.");
        }

        auto row_sign = get_max_abs(row_vec[0], row_vec[1]) > 0 ? 1 : -1;
        auto col_sign = get_max_abs(col_vec[0], col_vec[1]) > 0 ? 1 : -1;

        auto x_len = row_sign * get_length(row_vec);
        auto y_len = col_sign * get_length(col_vec);
        return {{origin[0], origin[1]}, {x_len, y_len}};
    }

    namespace
    {
        Xyz::Vector<size_t, 2> get_cell(const GridView& grid,
                                        const Xyz::Vector2F& grid_pos)
        {
            if (grid_pos[0] < 0 || float(grid.row_count() - 1) < grid_pos[0])
                GRIDLIB_THROW("Row index out of bounds.");
            if (grid_pos[1] < 0 || float(grid.col_count() - 1) < grid_pos[1])
                GRIDLIB_THROW("Column index out of bounds.");
            auto c = Xyz::vector_cast<size_t>(floor(grid_pos));
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
                                 Xyz::Vector2F& grid_pos,
                                 const Xyz::Vector<size_t, 2>& cell,
                                 const Xyz::Vector4F& values)
        {
            float ri;
            const auto rf = std::modf(grid_pos[0], &ri);
            float ci;
            const auto cf = std::modf(grid_pos[1], &ci);

            const auto r = size_t(ri) - cell[0];
            const auto c = size_t(ci) - cell[1];

            if (rf == 0 && cf == 0)
                return values[r * 2 + c];

            const auto unknown = *grid.unknown_elevation();
            if (rf == 0)
            {
                if (values[r * 2] != unknown && values[r * 2 + 1] != unknown)
                    return std::lerp(values[r * 2], values[r * 2 + 1], cf);
            }
            else if (cf == 0)
            {
                if (values[c] != unknown && values[2 + c] != unknown)
                    return std::lerp(values[c], values[2 + c], rf);
            }

            return unknown;
        }
    }

    float get_elevation(const GridView& grid, Xyz::Vector2F grid_pos)
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
        return bilinear(cell_values, p1, p2, grid_pos);
    }
}
