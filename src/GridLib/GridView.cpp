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
    namespace
    {
        const SpatialInfo DEFAULT_SPATIAL_INFO;
    }

    GridView::GridView() noexcept
        : GridView({}, nullptr, {})
    {
    };

    GridView::GridView(const Grid& grid) noexcept
        : GridView(grid.values(), &grid.spatial_info(), {})
    {
    }

    GridView::GridView(const Chorasmia::ArrayView2D<float>& elevations,
                       const SpatialInfo* spatial_info,
                       const Index& offset) noexcept
        : spatial_info_(spatial_info ? spatial_info : &DEFAULT_SPATIAL_INFO),
          values_(elevations),
          grid_offset_(offset)
    {
    }

    Size GridView::size() const
    {
        return {values_.row_count(), values_.col_count()};
    }

    Chorasmia::ArrayView2D<float> GridView::values() const
    {
        return values_;
    }

    const Index& GridView::grid_offset() const
    {
        return grid_offset_;
    }

    Xyz::Vector2D GridView::tie_point() const
    {
        return spatial_info_->tie_point + to_vector<double>(grid_offset_);
    }

    const SpatialInfo& GridView::spatial_info() const
    {
        return *spatial_info_;
    }

    GridView GridView::subgrid(const Index& index, const Size& size) const
    {
        assert_grid();
        auto [row, column] = index;
        auto [n_rows, n_cols] = size;
        return GridView(
            values_.subarray({{row, column}, {n_rows, n_cols}}),
            spatial_info_,
            grid_offset_ + index
        );
    }

    void GridView::assert_grid() const
    {
        if (!spatial_info_)
            GRIDLIB_THROW("grid is NULL");
    }
}
