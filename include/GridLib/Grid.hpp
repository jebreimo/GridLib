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
    class Grid : public IGridMetadata
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
        Chorasmia::ArrayView2D<float> elevations() const;

        [[nodiscard]]
        Chorasmia::MutableArrayView2D<float> elevations();

        [[nodiscard]]
        const Xyz::Vector2D& model_tie_point() const override;

        void set_model_tie_point(const Xyz::Vector2D& value);

        [[nodiscard]]
        const GridModel& model() const override;

        [[nodiscard]]
        GridModel& model();

        [[nodiscard]]
        const std::vector<SpatialTiePoint>& spatial_tie_points() const;

        void set_spatial_tie_points(std::vector<SpatialTiePoint> value);

        [[nodiscard]]
        GridView subgrid(size_t row, size_t column,
                         size_t n_rows = SIZE_MAX,
                         size_t n_cols = SIZE_MAX) const;

        [[nodiscard]]
        Chorasmia::Array2D<float> release();
    private:
        Chorasmia::Array2D<float> grid_;
        GridModel model_;
        Xyz::Vector2D model_tie_point_;
        std::vector<SpatialTiePoint> spatial_tie_points_;
    };

    bool operator==(const Grid& a, const Grid& b);

    bool operator!=(const Grid& a, const Grid& b);
}
