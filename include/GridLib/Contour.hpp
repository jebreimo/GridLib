//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <vector>
#include <Chorasmia/ArrayView2D.hpp>

namespace GridLib
{
    double getValue(Chorasmia::ArrayView2D<double> grid,
                    double x, double y);

    std::vector<double>
    getContour(Chorasmia::ArrayView2D<double> grid,
               double x, double y,
               double dx, double dy,
               size_t count);

}
