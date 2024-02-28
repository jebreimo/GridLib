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

        explicit Grid(Chorasmia::Array2D<double> values);

        void clear();

        [[nodiscard]]
        size_t row_count() const;

        [[nodiscard]]
        size_t col_count() const;

        void resize(size_t rows, size_t columns);

        [[nodiscard]]
        Chorasmia::ArrayView2D<double> elevations() const;

        Chorasmia::MutableArrayView2D<double> elevations();

        [[nodiscard]]
        std::optional<double> unknown_elevation() const;

        Grid& set_unknown_elevation(std::optional<double> value);

        [[nodiscard]]
        const Axis& row_axis() const;

        Grid& set_row_axis(const Axis& axis);

        [[nodiscard]]
        const Axis& column_axis() const;

        Grid& set_column_axis(const Axis& axis);

        [[nodiscard]]
        const Axis& vertical_axis() const;

        Grid& set_vertical_axis(const Axis& axis);

        [[nodiscard]]
        const std::optional<SphericalCoords>& spherical_coords() const;

        Grid& set_spherical_coords(const std::optional<SphericalCoords>& coords);

        [[nodiscard]]
        const std::optional<PlanarCoords>& planar_coords() const;

        Grid& set_planar_coords(const std::optional<PlanarCoords>& coords);

        [[nodiscard]]
        const std::optional<ReferenceSystem>& reference_system() const;

        Grid& set_reference_system(std::optional<ReferenceSystem> system);

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column,
                         size_t n_rows = SIZE_MAX,
                         size_t n_cols = SIZE_MAX) const;

        [[nodiscard]]
        Chorasmia::Array2D<double> release();
    private:
        Chorasmia::Array2D<double> grid_;
        std::optional<double> unknown_elevation_;
        Axis row_axis_;
        Axis column_axis_;
        Axis vertical_axis_;
        std::optional<SphericalCoords> spherical_coords_;
        std::optional<PlanarCoords> planar_coords_;
        std::optional<ReferenceSystem> reference_system_;
    };

    bool operator==(const Grid& a, const Grid& b);

    bool operator!=(const Grid& a, const Grid& b);
}
