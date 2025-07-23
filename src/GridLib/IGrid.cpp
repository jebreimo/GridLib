//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/IGrid.hpp"

#include <cfloat>
#include <Xyz/Interpolation.hpp>
#include "GridLib/Grid.hpp"
#include "GridLib/GridLibException.hpp"
#include "GridLib/PositionTransformer.hpp"

namespace GridLib
{
    GridView IGrid::subgrid(size_t row, size_t column) const
    {
        return subgrid(row, column, SIZE_MAX, SIZE_MAX);
    }

    std::pair<float, float> get_min_max_elevation(const IGrid& grid)
    {
        const auto elevations = grid.values();
        const auto no_value = grid.model().unknown_elevation;
        auto min = FLT_MAX, max = FLT_TRUE_MIN;
        for (const auto row : elevations)
        {
            for (const auto value : row)
            {
                if (no_value && value == *no_value)
                    continue;
                if (value < min)
                    min = value;
                if (value > max)
                    max = value;
            }
        }

        if (min > max)
            return {};

        return {min, max};
    }

    bool is_elevation_grid(const IGrid& grid)
    {
        const auto& m = grid.model();
        return m.row_axis()[2] <= Xyz::Margin<double>::DEFAULT
               && m.column_axis()[2] <= Xyz::Margin<double>::DEFAULT;
    }

    template <typename T, unsigned N>
    Xyz::Vector<T, N> get_min(const Xyz::Vector<T, N>& v,
                              const Xyz::Vector<T, N>& w)
    {
        Xyz::Vector<T, N> result;
        for (unsigned i = 0; i < N; ++i)
            result[i] = std::min(v[i], w[i]);
        return result;
    }

    template <typename T, unsigned N>
    Xyz::Vector<T, N> get_max(const Xyz::Vector<T, N>& v,
                              const Xyz::Vector<T, N>& w)
    {
        Xyz::Vector<T, N> result;
        for (unsigned i = 0; i < N; ++i)
            result[i] = std::max(v[i], w[i]);
        return result;
    }

    Xyz::Pgram3D get_bounds(const IGrid& grid)
    {
        if (!is_elevation_grid(grid))
            GRIDLIB_THROW("Can not calculate bounding rectangle for non-planar grid.");

        const PositionTransformer transformer(grid);
        const auto origin = transformer.grid_to_model({0, 0});
        const auto rows = grid.row_count();
        const auto cols = grid.col_count();
        const auto& m = grid.model();
        const auto col_vec = double(rows - 1) * m.column_axis();
        const auto row_vec = double(cols - 1) * m.row_axis();
        return {origin, col_vec, row_vec};
    }
}
