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

    Grid::Grid(size_t rows, size_t columns)
        : Grid(Chorasmia::Array2D<float>(rows, columns))
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

    size_t Grid::row_count() const
    {
        return grid_.row_count();
    }

    size_t Grid::col_count() const
    {
        return grid_.col_count();
    }

    void Grid::resize(size_t rows, size_t columns)
    {
        grid_.resize(rows, columns);
    }

    Chorasmia::ArrayView2D<float> Grid::values() const
    {
        return grid_.view();
    }

    Chorasmia::MutableArrayView2D<float> Grid::elevations()
    {
        return {grid_.data(), grid_.row_count(), grid_.col_count()};
    }

    const Xyz::Vector2D& Grid::model_tie_point() const
    {
        return model_tie_point_;
    }

    void Grid::set_model_tie_point(const Xyz::Vector2D& value)
    {
        model_tie_point_ = value;
    }

    const GridModel& Grid::model() const
    {
        return model_;
    }

    GridModel& Grid::model()
    {
        return model_;
    }

    std::vector<SpatialTiePoint> Grid::spatial_tie_points() const
    {
        return spatial_tie_points_;
    }

    void Grid::set_spatial_tie_points(std::vector<SpatialTiePoint> value)
    {
        spatial_tie_points_ = std::move(value);
    }

    GridView Grid::subgrid(size_t row, size_t column,
                           size_t n_rows, size_t n_cols) const
    {
        return GridView(*this).subgrid(row, column, n_rows, n_cols);
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
               && a.model_tie_point() == b.model_tie_point()
               && a.model() == b.model()
               && a.spatial_tie_points() == b.spatial_tie_points();
    }

    bool operator!=(const Grid& a, const Grid& b)
    {
        return !(a == b);
    }
}
