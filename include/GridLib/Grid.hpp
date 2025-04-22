//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-15.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
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

        explicit Grid(Chorasmia::Array2D<float> values);

        void clear();

        [[nodiscard]]
        bool empty() const;

        [[nodiscard]]
        GridView view() const;

        [[nodiscard]]
        size_t row_count() const;

        [[nodiscard]]
        size_t col_count() const;

        void resize(size_t rows, size_t columns);

        [[nodiscard]]
        Chorasmia::ArrayView2D<float> elevations() const;

        Chorasmia::MutableArrayView2D<float> elevations();

        [[nodiscard]]
        std::optional<float> unknown_elevation() const;

        Grid& set_unknown_elevation(std::optional<float> value);

        [[nodiscard]]
        const Xyz::Vector3D& row_axis() const;

        Grid& set_row_axis(const Xyz::Vector3D& axis);

        [[nodiscard]]
        const Xyz::Vector3D& column_axis() const;

        Grid& set_column_axis(const Xyz::Vector3D& axis);

        [[nodiscard]]
        const Xyz::Vector3D& vertical_axis() const;

        Grid& set_vertical_axis(const Xyz::Vector3D& axis);

        [[nodiscard]]
        const Coordinates& coordinates() const;

        Grid& set_coordinates(const Coordinates& coords);

        [[nodiscard]]
        const CoordinateReferenceSystem& reference_system() const;

        Grid& set_reference_system(const CoordinateReferenceSystem& system);

        [[nodiscard]]
        Unit horizontal_unit() const;

        Grid& set_horizontal_unit(Unit unit);

        [[nodiscard]]
        Unit vertical_unit() const;

        Grid& set_vertical_unit(Unit unit);

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column,
                         size_t n_rows = SIZE_MAX,
                         size_t n_cols = SIZE_MAX) const;

        [[nodiscard]]
        Chorasmia::Array2D<float> release();
    private:
        Chorasmia::Array2D<float> grid_;
        std::optional<float> unknown_elevation_;
        Xyz::Vector3D row_axis_;
        Xyz::Vector3D column_axis_;
        Xyz::Vector3D vertical_axis_;
        Unit horizontal_unit_ = Unit::UNDEFINED;
        Unit vertical_unit_ = Unit::UNDEFINED;
        Coordinates coordinates_;
        CoordinateReferenceSystem reference_system_;
    };

    bool operator==(const Grid& a, const Grid& b);

    bool operator!=(const Grid& a, const Grid& b);
}
