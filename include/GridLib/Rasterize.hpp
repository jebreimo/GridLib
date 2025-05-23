//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-03-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cfloat>
#include <concepts>
#include <Chorasmia/Index2DMap.hpp>
#include <Chorasmia/IntervalMap.hpp>
#include <Yimage/Image.hpp>
#include "IGrid.hpp"

namespace GridLib
{
    struct ElevationGradient
    {
        explicit ElevationGradient(Chorasmia::IntervalMap<float, uint32_t> color_map)
            : color_map_(std::move(color_map))
        {}

        uint32_t operator()(float elevation) const
        {
            if (auto it = color_map_.find(elevation); it != color_map_.end())
                return it->second;
            return 0;
        }

        Chorasmia::IntervalMap<float, uint32_t> color_map_;
    };

    Chorasmia::IntervalMap<float, uint32_t> make_map_gradient(
        float sea_level_min,
        float sea_level_max,
        float ground_level_1_max,
        float ground_level_2_max,
        float ground_level_3_max,
        float ground_level_4_max,
        float ground_level_5_max);

    Chorasmia::IntervalMap<float, uint32_t> make_default_gradient_2500();

    Chorasmia::IntervalMap<float, uint32_t> make_default_gradient_9000();

    template <typename T>
    concept ColorFunc = requires(T func, float value)
    {
        { func(value) } -> std::same_as<uint32_t>;
    };

    template <ColorFunc ColorFunc>
    Yimage::Image
    rasterize_rgba(const Chorasmia::ArrayView2D<float>& grid,
                   ColorFunc color_func,
                   Chorasmia::Index2DMode mode)
    {
        Chorasmia::Index2DMap mapping(grid.dimensions(), mode);
        auto [rows, cols] = mapping.get_to_size();
        Yimage::Image result(Yimage::PixelType::RGBA_8, cols, rows);
        auto* out_it = result.data();
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                auto rgba = color_func(grid(mapping.get_from_indices(i, j)));
                *out_it++ = rgba & 0xFF;
                *out_it++ = (rgba >> 8) & 0xFF;
                *out_it++ = (rgba >> 16) & 0xFF;
                *out_it++ = (rgba >> 24) & 0xFF;
            }
        }

        return result;
    }

    template <ColorFunc ColorFunc>
    Yimage::Image
    rasterize_rgba(const IGrid& grid, ColorFunc color_func,
                   Chorasmia::Index2DMode mode = Chorasmia::Index2DMode::ROWS)
    {
        return rasterize_rgba(grid.values(),
                              std::forward<ColorFunc>(color_func),
                              mode);
    }

    /**
     * @brief Returns the index mode to use when producing a bitmap with the
     *  origin in its top left corner.
     */
    Chorasmia::Index2DMode
    get_index_mode_for_top_left_origin(const IGrid& grid);
}
