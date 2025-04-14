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
    {}

    Grid::Grid(size_t rows, size_t columns)
        : Grid(Chorasmia::Array2D<float>(rows, columns))
    {}

    Grid::Grid(Chorasmia::Array2D<float> values)
        : grid_(std::move(values)),
          row_axis_{{1, 0, 0}, Unit::UNDEFINED},
          column_axis_{{0, -1, 0}, Unit::UNDEFINED},
          vertical_axis_{{0, 0, 1}, Unit::UNDEFINED}
    {}

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

    Chorasmia::ArrayView2D<float> Grid::elevations() const
    {
        return grid_.view();
    }

    Chorasmia::MutableArrayView2D<float> Grid::elevations()
    {
        return {grid_.data(), grid_.row_count(), grid_.col_count()};
    }

    std::optional<float> Grid::unknown_elevation() const
    {
        return unknown_elevation_;
    }

    Grid& Grid::set_unknown_elevation(std::optional<float> value)
    {
        unknown_elevation_ = value;
        return *this;
    }

    const Axis& Grid::row_axis() const
    {
        return row_axis_;
    }

    Grid& Grid::set_row_axis(const Axis& axis)
    {
        row_axis_ = axis;
        return *this;
    }

    const Axis& Grid::column_axis() const
    {
        return column_axis_;
    }

    Grid& Grid::set_column_axis(const Axis& axis)
    {
        column_axis_ = axis;
        return *this;
    }

    const Axis& Grid::vertical_axis() const
    {
        return vertical_axis_;
    }

    Grid& Grid::set_vertical_axis(const Axis& axis)
    {
        vertical_axis_ = axis;
        return *this;
    }

    const std::optional<SphericalCoords>& Grid::spherical_coords() const
    {
        return spherical_coords_;
    }

    Grid& Grid::set_spherical_coords(const std::optional<SphericalCoords>& coords)
    {
        spherical_coords_ = coords;
        return *this;
    }

    const std::optional<PlanarCoords>& Grid::planar_coords() const
    {
        return planar_coords_;
    }

    Grid& Grid::set_planar_coords(const std::optional<PlanarCoords>& coords)
    {
        planar_coords_ = coords;
        return *this;
    }

    const std::optional<ReferenceSystem>& Grid::reference_system() const
    {
        return reference_system_;
    }

    Grid& Grid::set_reference_system(std::optional<ReferenceSystem> system)
    {
        reference_system_ = system;
        return *this;
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
        return a.elevations() == b.elevations()
               && a.unknown_elevation() == b.unknown_elevation()
               && a.row_axis() == b.row_axis()
               && a.column_axis() == b.column_axis()
               && a.vertical_axis() == b.vertical_axis()
               && a.spherical_coords() == b.spherical_coords()
               && a.planar_coords() == b.planar_coords()
               && a.reference_system() == b.reference_system();
    }

    bool operator!=(const Grid& a, const Grid& b)
    {
        return !(a == b);
    }
}
