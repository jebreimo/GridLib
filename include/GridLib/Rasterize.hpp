//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-03-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cfloat>
#include <Chorasmia/Array2D.hpp>
#include <Chorasmia/Index2DMap.hpp>
#include <Chorasmia/IntervalMap.hpp>
#include "Grid.hpp"

namespace GridLib
{
    struct ElevationGradient
    {
        explicit ElevationGradient(Chorasmia::IntervalMap<double, uint32_t> colorMap)
            : m_ColorMap(std::move(colorMap))
        {}

        uint32_t operator()(double elevation) const
        {
            if (auto it = m_ColorMap.find(elevation); it != m_ColorMap.end())
                return it->second;
            return 0;
        }

        Chorasmia::IntervalMap<double, uint32_t> m_ColorMap;
    };

    Chorasmia::IntervalMap<double, uint32_t> makeMapGradient(
        double seaLevelMin,
        double seaLevelMax,
        double groundLevel1Max,
        double groundLevel2Max,
        double groundLevel3Max,
        double groundLevel4Max,
        double groundLevel5Max);

    Chorasmia::IntervalMap<double, uint32_t> makeDefaultGradient2500();

    Chorasmia::IntervalMap<double, uint32_t> makeDefaultGradient9000();

    template <typename ColorFunc>
    Chorasmia::Array2D<uint32_t>
    rasterizeRgba(const Chorasmia::ArrayView2D<double>& grid,
                  ColorFunc colorFunc,
                  Chorasmia::Index2DMode path)
    {
        Chorasmia::Index2DMap mapping(grid.dimensions(), path);
        auto [rows, cols] = mapping.getToSize();
        Chorasmia::Array2D<uint32_t> result(rows, cols);
        auto* outIt = result.data();
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
                *outIt++ = colorFunc(grid(mapping.getFromIndices(i, j)));
        }

        return result;
    }

    template <typename ColorFunc>
    Chorasmia::Array2D<uint32_t>
    rasterizeRgba(const GridView& grid, ColorFunc colorFunc,
                  Chorasmia::Index2DMode path = Chorasmia::Index2DMode::ROWS)
    {
        return rasterizeRgba(grid.elevations(),
                             std::forward<ColorFunc>(colorFunc),
                             path);
    }
}
