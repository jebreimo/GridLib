//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-15.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Chorasmia/Array2D.hpp>
#include "GridView.hpp"

namespace GridLib
{
    class Grid : public IGrid
    {
    public:
        Grid();

        explicit Grid(const Size& size);

        explicit Grid(Chorasmia::Array2D<float> values);

        void clear();

        [[nodiscard]]
        bool empty() const;

        [[nodiscard]]
        GridView view() const;

        [[nodiscard]]
        Size size() const override;

        void resize(const Size& size);

        [[nodiscard]]
        Chorasmia::ArrayView2D<float> values() const override;

        [[nodiscard]]
        Chorasmia::MutableArrayView2D<float> values();

        [[nodiscard]]
        float operator[](Index index) const
        {
            return values_(index.x(), index.y());
        }

        [[nodiscard]]
        float& operator[](Index index)
        {
            return values_(index.x(), index.y());
        }

        [[nodiscard]] Xyz::Vector2D tie_point() const override;

        [[nodiscard]]
        const SpatialInfo& spatial_info() const override;

        [[nodiscard]]
        SpatialInfo& spatial_info();

        [[nodiscard]]
        GridView subgrid(const Index& index, const Size& size) const override;

        [[nodiscard]]
        Chorasmia::Array2D<float> release();
    private:
        Chorasmia::Array2D<float> values_;
        SpatialInfo spatial_info_;
    };

    bool operator==(const Grid& a, const Grid& b);

    bool operator!=(const Grid& a, const Grid& b);
}
