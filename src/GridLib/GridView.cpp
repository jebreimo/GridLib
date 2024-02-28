//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
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
    {}

    GridView::GridView(const Grid& grid,
                       Chorasmia::ArrayView2D<double> elevations,
                       std::optional<SphericalCoords> spherical_coords,
                       std::optional<PlanarCoords> planar_coords) noexcept
        : grid_(&grid),
          elevations_(elevations),
          spherical_coords_(spherical_coords),
          planar_coords_(planar_coords)
    {}

    size_t GridView::row_count() const
    {
        return elevations_.row_count();
    }

    size_t GridView::col_count() const
    {
        return elevations_.col_count();
    }

    Chorasmia::ArrayView2D<double> GridView::elevations() const
    {
        return elevations_;
    }

    std::optional<double> GridView::unknown_elevation() const
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
        return {*grid_,
                elevations_.subarray(row, column, n_rows, n_cols),
                spherical_coords, planar_coords};
    }

    void GridView::assert_grid() const
    {
        if (!grid_)
            GRIDLIB_THROW("grid is NULL");
    }

    std::pair<double, double> get_min_max_elevation(const GridView& grid)
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
}
