//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IGrid.hpp"

namespace GridLib
{
    class Grid;

    class GridView : public IGrid
    {
    public:
        GridView() noexcept;

        explicit GridView(const Grid& grid) noexcept;

        explicit GridView(const Chorasmia::ArrayView2D<float>& elevations,
                          const SpatialInfo* spatial_info = nullptr,
                          const Index& offset = {}) noexcept;

        [[nodiscard]]
        Size size() const override;

        [[nodiscard]]
        const Index& grid_offset() const;

        [[nodiscard]]
        Xyz::Vector2D tie_point() const override;

        [[nodiscard]]
        const SpatialInfo& spatial_info() const override;

        [[nodiscard]]
        Chorasmia::ArrayView2D<float> values() const override;

        [[nodiscard]]
        float operator[](Index index) const
        {
            return values_(index.x(), index.y());
        }

        [[nodiscard]]
        GridView subgrid(const Index& index, const Size& size) const override;

    private:
        void assert_grid() const;

        const SpatialInfo* spatial_info_;
        Chorasmia::ArrayView2D<float> values_;
        Index grid_offset_;
    };
}
