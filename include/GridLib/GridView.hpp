//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <Chorasmia/ArrayView2D.hpp>
#include "GridMemberTypes.hpp"

namespace GridLib
{
    class Grid;

    class GridView
    {
    public:
        GridView() noexcept;

        GridView(const Grid& grid) noexcept;

        GridView(const Grid& grid,
                 Chorasmia::ArrayView2D<double> elevations,
                 std::optional<SphericalCoords> sphericalCoords,
                 std::optional<PlanarCoords> planarCoords) noexcept;

        [[nodiscard]]
        size_t rowCount() const;

        [[nodiscard]]
        size_t columnCount() const;

        [[nodiscard]]
        Chorasmia::ArrayView2D<double> elevations() const;

        [[nodiscard]]
        std::optional<double> unknownElevation() const;

        [[nodiscard]]
        const Axis& rowAxis() const;

        [[nodiscard]]
        const Axis& columnAxis() const;

        [[nodiscard]]
        const Axis& verticalAxis() const;

        [[nodiscard]]
        const std::optional<SphericalCoords>& sphericalCoords() const;

        [[nodiscard]]
        const std::optional<PlanarCoords>& planarCoords() const;

        [[nodiscard]]
        double rotationAngle() const;

        [[nodiscard]]
        RotationDir axisOrientation() const;

        [[nodiscard]]
        const std::optional<ReferenceSystem>& referenceSystem() const;

        const Grid* baseGrid() const;

        GridView subgrid(size_t row, size_t column,
                         size_t nrows, size_t ncolumns) const;
    private:
        void assertGrid() const;

        const Grid* m_Grid = nullptr;
        Chorasmia::ArrayView2D<double> m_Elevations;
        std::optional<SphericalCoords> m_SphericalCoords;
        std::optional<PlanarCoords> m_PlanarCoords;
    };
}
