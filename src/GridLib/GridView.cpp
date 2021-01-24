//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/GridView.hpp"
#include "GridLib/Grid.hpp"
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    GridView::GridView() noexcept = default;

    GridView::GridView(const Grid& grid) noexcept
        : GridView(grid,
                   grid.elevations(),
                   grid.sphericalCoords(),
                   grid.planarCoords())
    {}

    GridView::GridView(const Grid& grid,
                       Chorasmia::ArrayView2D<double> elevations,
                       std::optional<SphericalCoords> sphericalCoords,
                       std::optional<PlanarCoords> planarCoords) noexcept
        : m_Grid(&grid),
          m_Elevations(elevations),
          m_SphericalCoords(move(sphericalCoords)),
          m_PlanarCoords(move(planarCoords))
    {}

    size_t GridView::rowCount() const
    {
        return m_Elevations.rowCount();
    }

    size_t GridView::columnCount() const
    {
        return m_Elevations.columnCount();
    }

    Chorasmia::ArrayView2D<double> GridView::elevations() const
    {
        return m_Elevations;
    }

    std::optional<double> GridView::unknownElevation() const
    {
        assertGrid();
        return m_Grid->unknownElevation();
    }

    const Axis& GridView::rowAxis() const
    {
        assertGrid();
        return m_Grid->rowAxis();
    }

    const Axis& GridView::columnAxis() const
    {
        assertGrid();
        return m_Grid->columnAxis();
    }

    const Axis& GridView::verticalAxis() const
    {
        assertGrid();
        return m_Grid->verticalAxis();
    }

    const std::optional<SphericalCoords>& GridView::sphericalCoords() const
    {
        return m_SphericalCoords;
    }

    const std::optional<PlanarCoords>& GridView::planarCoords() const
    {
        return m_PlanarCoords;
    }

    double GridView::rotationAngle() const
    {
        assertGrid();
        return m_Grid->rotationAngle();
    }

    RotationDir GridView::axisOrientation() const
    {
        assertGrid();
        return m_Grid->axisOrientation();
    }

    const std::optional<ReferenceSystem>& GridView::referenceSystem() const
    {
        assertGrid();
        return m_Grid->referenceSystem();
    }

    const Grid* GridView::baseGrid() const
    {
        return m_Grid;
    }

    GridView GridView::subgrid(size_t row, size_t column,
                               size_t nrows, size_t ncolumns) const
    {
        assertGrid();
        auto planarCoords = m_PlanarCoords;
        if (planarCoords && (row != 0 || column != 0))
        {
            constexpr double METERS_PER_FOOT = 0.3048;
            std::pair<double, double> dr = {rowAxis().resolution, 0};
            std::pair<double, double> dc = {0, columnAxis().resolution};
            if (rowAxis().unit == Unit::FEET)
                dr.first /= METERS_PER_FOOT;
            if (columnAxis().unit == Unit::FEET)
                dc.second /= METERS_PER_FOOT;
            if (axisOrientation() == RotationDir::CLOCKWISE)
                dc.second = -dc.second;
            if (rotationAngle() != 0)
            {
                dr = {dr.first * cos(rotationAngle()), dr.first * sin(rotationAngle())};
                dc = {-dc.second * sin(rotationAngle()), dc.second * cos(rotationAngle())};
            }
            planarCoords->easting += dr.first * row + dc.first * column;
            planarCoords->northing += dr.second * row + dc.second * column;
        }
        auto sphericalCoords = row == 0 && column == 0
                               ? m_SphericalCoords
                               : std::optional<SphericalCoords>();
        return GridView(*m_Grid,
                        m_Elevations.subarray(row, column, nrows, ncolumns),
                        sphericalCoords, planarCoords);
    }

    void GridView::assertGrid() const
    {
        if (!m_Grid)
            GRIDLIB_THROW("grid is NULL");
    }
}
