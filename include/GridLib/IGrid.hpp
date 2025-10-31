//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-04-24.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Chorasmia/ArrayView2D.hpp>
#include <Xyz/Pgram3.hpp>
#include "GridLib/SpatialInfo.hpp"

namespace GridLib
{
    class GridView;

    using Size = Xyz::Vector<int64_t, 2>;
    using Index = Xyz::Vector<int64_t, 2>;

    class IGrid
    {
    public:
        virtual ~IGrid() = default;

        [[nodiscard]]
        virtual Size size() const = 0;

        [[nodiscard]]
        virtual Xyz::Vector2D tie_point() const = 0;

        [[nodiscard]]
        virtual const SpatialInfo& spatial_info() const = 0;

        [[nodiscard]]
        virtual Chorasmia::ArrayView2D<float> values() const = 0;

        [[nodiscard]]
        GridView subgrid(const Index& index) const;

        [[nodiscard]]
        virtual GridView subgrid(const Index& index,
                                 const Size& size) const = 0;
    };

    [[nodiscard]]
    std::pair<float, float> get_min_max_elevation(const IGrid& grid);

    [[nodiscard]]
    bool is_elevation_grid(const IGrid& grid);

    [[nodiscard]]
    Xyz::Pgram3D get_bounds(const IGrid& grid);
}
