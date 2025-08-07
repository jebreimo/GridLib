//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-15.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Grid.hpp"

namespace GridLib
{
    Grid::Grid() = default;

    Grid::Grid(const Size& size)
        : values_(size.rows, size.cols)
    {
        values_.fill(UNKNOWN_ELEVATION);
    }

    Grid::Grid(Chorasmia::Array2D<float> values)
        : values_(std::move(values))
    {
    }

    void Grid::clear()
    {
        values_.fill(UNKNOWN_ELEVATION);
    }

    bool Grid::empty() const
    {
        return values_.empty();
    }

    GridView Grid::view() const
    {
        return GridView(*this);
    }

    Size Grid::size() const
    {
        return {
            static_cast<int64_t>(values_.row_count()),
            static_cast<int64_t>(values_.col_count())
        };
    }

    void Grid::resize(const Size& size)
    {
        values_.resize(size.rows, size.cols);
    }

    Chorasmia::ArrayView2D<float> Grid::values() const
    {
        return values_.view();
    }

    Chorasmia::MutableArrayView2D<float> Grid::values()
    {
        return {values_.data(), values_.row_count(), values_.col_count()};
    }

    const Xyz::Vector2D& Grid::tie_point() const
    {
        return tie_point_;
    }

    void Grid::set_tie_point(const Xyz::Vector2D& value)
    {
        tie_point_ = value;
    }

    const SpatialInfo& Grid::spatial_info() const
    {
        return spatial_info_;
    }

    SpatialInfo& Grid::spatial_info()
    {
        return spatial_info_;
    }

    std::vector<SpatialTiePoint> Grid::spatial_tie_points() const
    {
        return spatial_tie_points_;
    }

    void Grid::set_spatial_tie_points(std::vector<SpatialTiePoint> value)
    {
        spatial_tie_points_ = std::move(value);
    }

    GridView Grid::subgrid(const Index& index, const Size& size) const
    {
        return GridView(*this).subgrid(index, size);
    }

    Chorasmia::Array2D<float> Grid::release()
    {
        return std::move(values_);
    }

    bool operator==(const Grid& a, const Grid& b)
    {
        if (&a == &b)
            return true;
        return a.values() == b.values()
               && a.tie_point() == b.tie_point()
               && a.spatial_info() == b.spatial_info()
               && a.spatial_tie_points() == b.spatial_tie_points();
    }

    bool operator!=(const Grid& a, const Grid& b)
    {
        return !(a == b);
    }
}
