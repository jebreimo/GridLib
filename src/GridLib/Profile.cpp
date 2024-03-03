//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Profile.hpp"
#include <cmath>
#include <Chorasmia/ArrayView2DAlgorithms.hpp>
#include <Xyz/Vector.hpp>
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    double get_value(const GridView& grid, const Xyz::Vector2D& pos)
    {
        return interpolate_value(grid.elevations(), pos[0], pos[1]);
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
                auto z = Chorasmia::interpolate_value(grid, pos[0], pos[1]);
                result.push_back(Xyz::make_vector3(pos, z));
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
