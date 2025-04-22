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
        : grid_(std::move(values)),
          row_axis_{1, 0, 0},
          column_axis_{0, -1, 0},
          vertical_axis_{0, 0, 1}
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

    const Xyz::Vector3D& Grid::row_axis() const
    {
        return row_axis_;
    }

    Grid& Grid::set_row_axis(const Xyz::Vector3D& axis)
    {
        row_axis_ = axis;
        return *this;
    }

    const Xyz::Vector3D& Grid::column_axis() const
    {
        return column_axis_;
    }

    Grid& Grid::set_column_axis(const Xyz::Vector3D& axis)
    {
        column_axis_ = axis;
        return *this;
    }

    const Xyz::Vector3D& Grid::vertical_axis() const
    {
        return vertical_axis_;
    }

    Grid& Grid::set_vertical_axis(const Xyz::Vector3D& axis)
    {
        vertical_axis_ = axis;
        return *this;
    }

    Unit Grid::horizontal_unit() const
    {
        return horizontal_unit_;
    }

    Grid& Grid::set_horizontal_unit(Unit unit)
    {
        horizontal_unit_ = unit;
        return *this;
    }

    Unit Grid::vertical_unit() const
    {
        return vertical_unit_;
    }

    Grid& Grid::set_vertical_unit(Unit unit)
    {
        vertical_unit_ = unit;
        return *this;
    }

    const Coordinates& Grid::coordinates() const
    {
        return coordinates_;
    }

    Grid& Grid::set_coordinates(const Coordinates& coords)
    {
        coordinates_ = coords;
        return *this;
    }

    const CoordinateReferenceSystem& Grid::reference_system() const
    {
        return reference_system_;
    }

    Grid& Grid::set_reference_system(const CoordinateReferenceSystem& system)
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
               && a.horizontal_unit() == b.horizontal_unit()
               && a.vertical_unit() == b.vertical_unit()
               && a.coordinates() == b.coordinates()
               && a.reference_system() == b.reference_system();
    }

    bool operator!=(const Grid& a, const Grid& b)
    {
        return !(a == b);
    }
}
