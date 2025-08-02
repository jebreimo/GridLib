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
                   grid.tie_point())
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

    Size GridView::size() const
    {
        return {elevations_.row_count(), elevations_.col_count()};
    }

    Chorasmia::ArrayView2D<float> GridView::values() const
    {
        return elevations_;
    }

    const Xyz::Vector2D& GridView::tie_point() const
    {
        return model_tie_point_;
    }

    const SpatialInfo& GridView::spatial_info() const
    {
        assert_grid();
        return grid_->spatial_info();
    }

    std::vector<SpatialTiePoint> GridView::spatial_tie_points() const
    {
        assert_grid();
        if (model_tie_point_ == grid_->tie_point())
            return grid_->spatial_tie_points();

        const auto delta = model_tie_point_ - grid_->tie_point();

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

    GridView GridView::subgrid(const Index& index, const Size& size) const
    {
        assert_grid();
        auto [row, column] = index;
        auto [n_rows, n_cols] = size;
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
