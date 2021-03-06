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
            auto offset = row * columnAxis().direction
                          + column * rowAxis().direction;
            planarCoords->easting += offset[0];
            planarCoords->northing += offset[1];
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

    std::pair<double, double> getMinMaxElevation(const GridView& grid)
    {
        const auto elevations = grid.elevations();
        const auto noValue = grid.unknownElevation();
        auto min = DBL_MAX, max = DBL_TRUE_MIN;
        for (const auto row : elevations)
        {
            for (const auto value : row)
            {
                if (noValue && value == *noValue)
                    continue;
                if (value < min)
                    min = value;
                if (value > max)
                    max = value;
            }
        }

        if (min > max)
            return {};

        return {min, max};
    }
}
