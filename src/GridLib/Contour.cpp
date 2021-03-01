//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Contour.hpp"
#include <cmath>
#include <Xyz/Vector.hpp>
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    //double getValue(Chorasmia::ArrayView2D<double> grid, double x, double y)
    //{
    //    double x0i;
    //    const auto x0f = std::modf(x, &x0i);
    //    const auto x0f2 = x0f * x0f;
    //    const auto x1f = 1 - x0f;
    //    const auto x1f2 = x1f * x1f;
    //    double y0i;
    //    const auto y0f = std::modf(y, &y0i);
    //    const auto y0f2 = y0f * y0f;
    //    const auto y1f = 1 - y0f;
    //    const auto y1f2 = y1f * y1f;
    //    double z = 0;
    //    if (x0f2 + y0f2 < 1)
    //        z += grid(size_t(x0i), size_t(y0i)) * std::sqrt(1 - x0f2 - y0f2);
    //    if (x1f2 + y0f2 < 1)
    //        z += grid(size_t(x0i) + 1, size_t(y0i)) * std::sqrt(1 - x1f2 - y0f2);
    //    if (x0f2 + y1f2 < 1)
    //        z += grid(size_t(x0i), size_t(y0i) + 1) * std::sqrt(1 - x0f2 - y1f2);
    //    if (x1f2 + y1f2 < 1)
    //        z += grid(size_t(x0i) + 1, size_t(y0i) + 1) * std::sqrt(1 - x1f2 - y1f2);
    //    return z;
    //}

    double getValue(Chorasmia::ArrayView2D<double> grid, double x, double y)
    {
        double xi;
        auto xf = std::modf(x, &xi);
        const auto xs = size_t(xi);
        if (xi < 0 || grid.columnCount() - 1 <= xs)
        {
            if (grid.columnCount() - 1 != xs || xf != 0)
                GRIDLIB_THROW("x is outside the grid.");
        }
        double yi;
        auto yf = std::modf(y, &yi);
        const auto ys = size_t(yi);
        if (yi < 0 || grid.columnCount() - 1 <= ys)
        {
            if (grid.columnCount() - 1 != ys || yf != 0)
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

    std::vector<Xyz::Vector3d>
    getContour(Chorasmia::ArrayView2D<double> grid,
               Xyz::Vector2d from,
               Xyz::Vector2d to,
               size_t steps)
    {
        std::vector<Xyz::Vector3d> result;
        auto delta = to - from;
        auto step = delta / double(steps);
        for (size_t i = 0; i <= steps; ++i)
        {
            auto pos = from + double(i) * step;
            if (0 <= pos[0] <= grid.rowCount() && 0 <= pos[1] <= grid.columnCount())
            {
                result.push_back(Xyz::makeVector3(pos, getValue(grid, pos[0], pos[1])));
            }
        }
        return result;
    }

    std::vector<double>
    getContour(Chorasmia::ArrayView2D<double> grid,
               double x, double y, double dx, double dy,
               size_t count)
    {
        std::vector<double> result;
        for (size_t i = 0; i < count; ++i)
        {
            result.push_back(getValue(grid, x, y));
            x += dx;
            y += dy;
        }
        return result;
    }
}
