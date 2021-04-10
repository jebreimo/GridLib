//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-15.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Grid.hpp"

namespace GridLib
{
    Grid::Grid()
        : Grid(Chorasmia::Array2D<double>())
    {}

    Grid::Grid(size_t rows, size_t columns)
        : Grid(Chorasmia::Array2D<double>(rows, columns))
    {}

    Grid::Grid(Chorasmia::Array2D<double> values)
        : m_Grid(std::move(values)),
          m_RowAxis{{1, 0, 0}, Unit::METERS},
          m_ColumnAxis{{0, -1, 0}, Unit::METERS},
          m_VerticalAxis{{0, 0, 1}, Unit::METERS}
    {}

    void Grid::clear()
    {
        m_Grid.fill(0);
    }

    size_t Grid::rowCount() const
    {
        return m_Grid.rowCount();
    }

    size_t Grid::columnCount() const
    {
        return m_Grid.columnCount();
    }

    void Grid::resize(size_t rows, size_t columns)
    {
        m_Grid.resize(rows, columns);
    }

    Chorasmia::ArrayView2D<double> Grid::elevations() const
    {
        return m_Grid;
    }

    Chorasmia::MutableArrayView2D<double> Grid::elevations()
    {
        return {m_Grid.data(), m_Grid.rowCount(), m_Grid.columnCount()};
    }

    std::optional<double> Grid::unknownElevation() const
    {
        return m_UnknownElevation;
    }

    Grid& Grid::setUnknownElevation(std::optional<double> elevation)
    {
        m_UnknownElevation = elevation;
        return *this;
    }

    const Axis& Grid::rowAxis() const
    {
        return m_RowAxis;
    }

    Grid& Grid::setRowAxis(const Axis& axis)
    {
        m_RowAxis = axis;
        return *this;
    }

    const Axis& Grid::columnAxis() const
    {
        return m_ColumnAxis;
    }

    Grid& Grid::setColumnAxis(const Axis& axis)
    {
        m_ColumnAxis = axis;
        return *this;
    }

    const Axis& Grid::verticalAxis() const
    {
        return m_VerticalAxis;
    }

    Grid& Grid::setVerticalAxis(const Axis& axis)
    {
        m_VerticalAxis = axis;
        return *this;
    }

    const std::optional<SphericalCoords>& Grid::sphericalCoords() const
    {
        return m_SphericalCoords;
    }

    Grid& Grid::setSphericalCoords(const std::optional<SphericalCoords>& coords)
    {
        m_SphericalCoords = coords;
        return *this;
    }

    const std::optional<PlanarCoords>& Grid::planarCoords() const
    {
        return m_PlanarCoords;
    }

    Grid& Grid::setPlanarCoords(const std::optional<PlanarCoords>& coords)
    {
        m_PlanarCoords = coords;
        return *this;
    }

    const std::optional<ReferenceSystem>& Grid::referenceSystem() const
    {
        return m_ReferenceSystem;
    }

    Grid& Grid::setReferenceSystem(std::optional<ReferenceSystem> system)
    {
        m_ReferenceSystem = system;
        return *this;
    }

    GridView Grid::subgrid(size_t row, size_t column,
                           size_t nrows, size_t ncolumns) const
    {
        return GridView(*this).subgrid(row, column, nrows, ncolumns);
    }

    Chorasmia::Array2D<double> Grid::release()
    {
        return std::move(m_Grid);
    }

    bool operator==(const Grid& a, const Grid& b)
    {
        if (&a == &b)
            return true;
        return a.elevations() == b.elevations()
               && a.unknownElevation() == b.unknownElevation()
               && a.rowAxis() == b.rowAxis()
               && a.columnAxis() == b.columnAxis()
               && a.verticalAxis() == b.verticalAxis()
               && a.sphericalCoords() == b.sphericalCoords()
               && a.planarCoords() == b.planarCoords()
               && a.referenceSystem() == b.referenceSystem();
    }

    bool operator!=(const Grid& a, const Grid& b)
    {
        return !(a == b);
    }
}
