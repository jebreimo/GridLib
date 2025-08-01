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
    class Grid : public IGrid
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
        size_t row_count() const override;

        [[nodiscard]]
        size_t col_count() const override;

        void resize(size_t rows, size_t columns);

        [[nodiscard]]
        Chorasmia::ArrayView2D<float> values() const override;

        [[nodiscard]]
        Chorasmia::MutableArrayView2D<float> values();

        [[nodiscard]]
        const Xyz::Vector2D& tie_point() const override;

        void set_tie_point(const Xyz::Vector2D& value);

        [[nodiscard]]
        const SpatialInfo& spatial_info() const override;

        [[nodiscard]]
        SpatialInfo& spatial_info();

        [[nodiscard]]
        std::vector<SpatialTiePoint> spatial_tie_points() const override;

        void set_spatial_tie_points(std::vector<SpatialTiePoint> value);

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column,
                         size_t n_rows, size_t n_cols) const override;

        [[nodiscard]]
        Chorasmia::Array2D<float> release();
    private:
        Chorasmia::Array2D<float> grid_;
        SpatialInfo spatial_info_;
        Xyz::Vector2D tie_point_;
        std::vector<SpatialTiePoint> spatial_tie_points_;
    };

    bool operator==(const Grid& a, const Grid& b);

    bool operator!=(const Grid& a, const Grid& b);
}
