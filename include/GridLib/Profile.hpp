//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <vector>
#include <Xyz/Vector.hpp>
#include "GridView.hpp"

namespace GridLib
{
    double get_value(const GridView& grid, const Xyz::Vector2D& pos);

    std::vector<Xyz::Vector3D>
    make_profile(Chorasmia::ArrayView2D<double> grid,
                 const Xyz::Vector2D& from,
                 const Xyz::Vector2D& to,
                 size_t steps);

    std::vector<Xyz::Vector3D>
    make_profile(const GridView& grid,
                 const Xyz::Vector2D& from,
                 const Xyz::Vector2D& to,
                 size_t steps);
}
