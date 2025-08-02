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
    Grid::Grid()
        : Grid(Chorasmia::Array2D<float>())
    {
    }

    Grid::Grid(const Size& size)
        : Grid(Chorasmia::Array2D<float>(size.rows, size.cols))
    {
    }

    Grid::Grid(Chorasmia::Array2D<float> values)
        : grid_(std::move(values))
    {
    }

    void Grid::clear()
    {
        grid_.fill(0);
    }

    bool Grid::empty() const
    {
        return grid_.empty();
    }

    GridView Grid::view() const
    {
        return GridView(*this);
    }

    Size Grid::size() const
    {
        return {grid_.row_count(), grid_.col_count()};
    }

    void Grid::resize(const Size& size)
    {
        grid_.resize(size.rows, size.cols);
    }

    Chorasmia::ArrayView2D<float> Grid::values() const
    {
        return grid_.view();
    }

    Chorasmia::MutableArrayView2D<float> Grid::values()
    {
        return {grid_.data(), grid_.row_count(), grid_.col_count()};
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
        return std::move(grid_);
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
