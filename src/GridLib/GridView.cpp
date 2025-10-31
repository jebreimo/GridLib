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
                   Size{0, 0})
    {
    }

    GridView::GridView(const Grid& grid,
                       const Chorasmia::ArrayView2D<float>& elevations,
                       const Size& offset) noexcept
        : grid_(&grid),
          values_(elevations),
          grid_offset_(offset)
    {
    }

    Size GridView::size() const
    {
        return {
            static_cast<int64_t>(values_.row_count()),
            static_cast<int64_t>(values_.col_count())
        };
    }

    Chorasmia::ArrayView2D<float> GridView::values() const
    {
        return values_;
    }

    const Size& GridView::grid_offset() const
    {
        return grid_offset_;
    }

    Xyz::Vector2D GridView::tie_point() const
    {
        assert_grid();
        return grid_->tie_point() + vector_cast<double>(grid_offset_);
    }

    const SpatialInfo& GridView::spatial_info() const
    {
        assert_grid();
        return grid_->spatial_info();
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
            values_.subarray(row, column, n_rows, n_cols),
            grid_offset_ + index
        };
    }

    void GridView::assert_grid() const
    {
        if (!grid_)
            GRIDLIB_THROW("grid is NULL");
    }
}
