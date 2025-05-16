//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <Xyz/Interpolation.hpp>
#include "GridLib/Grid.hpp"
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    GridView::GridView() noexcept = default;

    GridView::GridView(const Grid& grid) noexcept
        : GridView(grid,
                   grid.values(),
                   grid.model_tie_point())
    {
    }

    GridView::GridView(const Grid& grid,
                       const Chorasmia::ArrayView2D<float>& elevations,
                       const Xyz::Vector2D& model_tie_point) noexcept
        : grid_(&grid),
          elevations_(elevations),
          model_tie_point_(model_tie_point)
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

    Chorasmia::ArrayView2D<float> GridView::values() const
    {
        return elevations_;
    }

    const Xyz::Vector2D& GridView::model_tie_point() const
    {
        return model_tie_point_;
    }

    const GridModel& GridView::model() const
    {
        assert_grid();
        return grid_->model();
    }

    std::vector<SpatialTiePoint> GridView::spatial_tie_points() const
    {
        assert_grid();
        if (model_tie_point_ == grid_->model_tie_point())
            return grid_->spatial_tie_points();

        const auto delta = model_tie_point_ - grid_->model_tie_point();

        std::vector<SpatialTiePoint> result;
        for (const auto& pt : grid_->spatial_tie_points())
        {
            result.push_back({
                pt.grid_point + delta,
                pt.location,
                pt.crs
            });
        }
        return result;
    }

    const Grid* GridView::base_grid() const
    {
        return grid_;
    }

    GridView GridView::subgrid(size_t row, size_t column,
                               size_t n_rows, size_t n_cols) const
    {
        assert_grid();
        return {
            *grid_,
            elevations_.subarray(row, column, n_rows, n_cols),
            model_tie_point_ - Xyz::Vector2D{double(row), double(column)}
        };
    }

    void GridView::assert_grid() const
    {
        if (!grid_)
            GRIDLIB_THROW("grid is NULL");
    }
}
