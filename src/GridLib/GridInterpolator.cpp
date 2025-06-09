//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-12.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/GridInterpolator.hpp"
#include <Xyz/Interpolation.hpp>
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    namespace
    {
        std::optional<Xyz::Vector<size_t, 2>>
        get_cell(const Chorasmia::ArrayView2D<float>& values,
                 const Xyz::Vector2D& grid_pos)
        {
            if (grid_pos[0] < 0 || double(values.row_count() - 1) < grid_pos[0])
                return {};
            if (grid_pos[1] < 0 || double(values.col_count() - 1) < grid_pos[1])
                return {};
            const auto c = Xyz::vector_cast<size_t>(floor(grid_pos));
            return get_clamped(c, {0, 0}, {values.row_count() - 2, values.col_count() - 2});
        }

        Xyz::Vector4F get_grid_cell_values(const Chorasmia::ArrayView2D<float>& values,
                                           const Xyz::Vector<size_t, 2>& cell)
        {
            return {
                values(cell[0], cell[1]),
                values(cell[0] + 1, cell[1]),
                values(cell[0], cell[1] + 1),
                values(cell[0] + 1, cell[1] + 1)
            };
        }

        bool has_unknown_value(const Xyz::Vector4F& values,
                               const std::optional<float>& unknown)
        {
            return unknown
                   && (values[0] == *unknown || values[1] == *unknown
                       || values[2] == *unknown || values[3] == *unknown);
        }

        std::optional<float>
        get_edge_elevation(const Xyz::Vector4F& values,
                           const Xyz::Vector2D& grid_pos,
                           const Xyz::Vector<size_t, 2>& cell,
                           const std::optional<float>& unknown)
        {
            double ri;
            const auto rf = std::modf(grid_pos[0], &ri);
            double ci;
            const auto cf = std::modf(grid_pos[1], &ci);

            const auto r = unsigned(ri) - unsigned(cell[0]);
            const auto c = unsigned(ci) - unsigned(cell[1]);

            if (rf == 0 && cf == 0)
            {
                auto v = values[r + c * 2];
                if (v == *unknown)
                    return {};
                return v;
            }

            if (rf == 0)
            {
                if (values[r] != *unknown && values[2 + r] != *unknown)
                    return std::lerp(values[r], values[2 + r], float(cf));
            }
            else if (cf == 0)
            {
                if (values[2 * c] != *unknown && values[2 * c + 1] != *unknown)
                    return std::lerp(values[2 * c], values[2 * c + 1], float(rf));
            }

            return {};
        }

        std::optional<float>
        interpolate_value(const IGrid& grid, const Xyz::Vector2D& grid_pos)
        {
            const auto values = grid.values();
            const auto cell = get_cell(values, grid_pos);
            if (!cell)
                return {};

            const auto cell_values = get_grid_cell_values(values, *cell);
            const auto& unknown = grid.model().unknown_elevation;
            if (has_unknown_value(cell_values, unknown))
                return get_edge_elevation(cell_values, grid_pos, *cell, unknown);

            const auto p1 = Xyz::vector_cast<double>(*cell);
            const auto p2 = p1 + Xyz::Vector2D(1, 1);
            return bilinear(cell_values, grid_pos, p1, p2);
        }
    }

    GridInterpolator::GridInterpolator(const IGrid& grid)
        : transformer(grid)
    {
    }

    std::optional<double> GridInterpolator::raw_value_at_grid_pos(
        const Xyz::Vector2D& grid_pos) const
    {
        return interpolate_value(transformer.grid, grid_pos);
    }

    std::optional<double> GridInterpolator::raw_value_at_model_pos(
        const Xyz::Vector3D& model_pos) const
    {
        const auto p = transformer.model_to_grid(model_pos);
        return raw_value_at_grid_pos(p);
    }

    std::optional<Xyz::Vector3D>
    GridInterpolator::at_grid_pos(const Xyz::Vector2D& grid_pos) const
    {
        if (const auto z = raw_value_at_grid_pos(grid_pos))
            return transformer.grid_to_model({grid_pos[0], grid_pos[1], *z});
        return {};
    }

    std::optional<Xyz::Vector3D>
    GridInterpolator::at_model_pos(const Xyz::Vector3D& model_pos) const
    {
        const auto p = transformer.model_to_grid(model_pos);
        if (const auto z = raw_value_at_grid_pos(p))
            return transformer.grid_to_model({p[0], p[1], *z});
        return {};
    }
}
