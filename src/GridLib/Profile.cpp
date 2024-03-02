//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Profile.hpp"
#include <cmath>
#include <Xyz/Vector.hpp>
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    double get_value(Chorasmia::ArrayView2D<double> grid, double x, double y)
    {
        double xi;
        auto xf = std::modf(x, &xi);
        const auto xs = size_t(xi);
        if (xi < 0 || grid.col_count() - 1 <= xs)
        {
            if (grid.col_count() - 1 != xs || xf != 0)
                GRIDLIB_THROW("x is outside the grid.");
        }
        double yi;
        auto yf = std::modf(y, &yi);
        const auto ys = size_t(yi);
        if (yi < 0 || grid.col_count() - 1 <= ys)
        {
            if (grid.col_count() - 1 != ys || yf != 0)
                GRIDLIB_THROW("y is outside the grid.");
        }

        auto z0 = grid(xs, ys);
        auto z1 = grid(xs + 1, ys);
        auto z2 = grid(xs, ys + 1);
        auto z3 = grid(xs + 1, ys + 1);

        if (std::abs(z3 - z0) >= std::abs(z2 - z1))
        {
            if (xf >= yf)
                return z1 + (1 - xf) * (z0 - z1) + yf * (z3 - z1);
            else
                return z2 + xf * (z3 - z2) + (1 - yf) * (z0 - z2);
        }
        else
        {
            if (xf <= 1 - yf)
                return z0 + xf * (z1 - z0) + yf * (z2 - z0);
            else
                return z3 + (1 - xf) * (z2 - z3) + (1 - yf) * (z1 - z3);
        }
    }

    std::vector<Xyz::Vector3D>
    make_profile(Chorasmia::ArrayView2D<double> grid,
                 const Xyz::Vector2D& from,
                 const Xyz::Vector2D& to,
                 size_t steps)
    {
        std::vector<Xyz::Vector3D> result;
        auto delta = to - from;
        auto step = delta / double(steps);
        for (size_t i = 0; i <= steps; ++i)
        {
            auto pos = from + double(i) * step;
            if (0 <= pos[0] && size_t(pos[0]) + 1 <= grid.row_count()
                && 0 <= pos[1] && size_t(pos[1]) + 1 <= grid.col_count())
            {
                result.push_back(Xyz::make_vector3(pos, get_value(grid, pos[0], pos[1])));
            }
        }
        return result;
    }

    std::vector<Xyz::Vector3D>
    make_profile(const GridView& grid,
                 const Xyz::Vector2D& from,
                 const Xyz::Vector2D& to,
                 size_t steps)
    {
        return make_profile(grid.elevations(), from, to, steps);
    }
}
