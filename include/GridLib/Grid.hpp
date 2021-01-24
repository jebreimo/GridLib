//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-15.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <variant>
#include <Chorasmia/Array2D.hpp>
#include "GridMemberTypes.hpp"
#include "GridView.hpp"

namespace GridLib
{
    class Grid
    {
    public:
        Grid();

        Grid(size_t rows, size_t columns);

        void clear();

        [[nodiscard]]
        size_t rowCount() const;

        [[nodiscard]]
        size_t columnCount() const;

        void resize(size_t rows, size_t columns);

        [[nodiscard]]
        Chorasmia::ArrayView2D<double> elevations() const;

        Chorasmia::MutableArrayView2D<double> elevations();

        [[nodiscard]]
        std::optional<double> unknownElevation() const;

        Grid& setUnknownElevation(std::optional<double> value);

        [[nodiscard]]
        const Axis& rowAxis() const;

        Grid& setRowAxis(const Axis& axis);

        [[nodiscard]]
        const Axis& columnAxis() const;

        Grid& setColumnAxis(const Axis& axis);

        [[nodiscard]]
        const Axis& verticalAxis() const;

        Grid& setVerticalAxis(const Axis& axis);

        [[nodiscard]]
        const std::optional<SphericalCoords>& sphericalCoords() const;

        Grid& setSphericalCoords(const std::optional<SphericalCoords>& coords);

        [[nodiscard]]
        const std::optional<PlanarCoords>& planarCoords() const;

        Grid& setPlanarCoords(const std::optional<PlanarCoords>& coords);

        [[nodiscard]]
        double rotationAngle() const;

        /**
         * @brief Set the ccw angle, in radians, that the row (major) axis
         * has been rotated relative to due east.
         *
         * @param angle The counter-clockwise angle from due east in radians.
         */
        Grid& setRotationAngle(double angle);

        [[nodiscard]]
        RotationDir axisOrientation() const;

        /**
         * @brief Set the orientation of the column (minor) axis relative to
         *  the row (major) axis.
         */
        Grid& setAxisOrientation(RotationDir dir);

        [[nodiscard]]
        const std::optional<ReferenceSystem>& referenceSystem() const;

        Grid& setReferenceSystem(std::optional<ReferenceSystem> system);

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column,
                         size_t nrows = SIZE_MAX,
                         size_t ncolumns = SIZE_MAX) const;

        [[nodiscard]]
        Chorasmia::Array2D<double> release();
    private:
        Chorasmia::Array2D<double> m_Grid;
        std::optional<double> m_UnknownElevation;
        Axis m_Axis[3];
        std::optional<SphericalCoords> m_SphericalCoords;
        std::optional<PlanarCoords> m_PlanarCoords;
        double m_RotationAngle = 0;
        std::optional<ReferenceSystem> m_ReferenceSystem;
        RotationDir m_AxisOrientation = RotationDir::COUNTERCLOCKWISE;
    };

    bool operator==(const Grid& a, const Grid& b);
}
