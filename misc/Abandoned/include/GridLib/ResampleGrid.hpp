//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "GridLib/Grid.hpp"

namespace GridLib
{
    Chorasmia::Array2D<double>
    resample(const Chorasmia::ArrayView2D<double>& grid,
             size_t rows, size_t columns);

    Chorasmia::Array2D<double>
    resampleNearestNeighbor(const Chorasmia::ArrayView2D<double>& grid,
                            size_t rows, size_t columns);

    Chorasmia::Array2D<double>
    resampleJeb(const Chorasmia::ArrayView2D<double>& grid,
                            size_t rows, size_t columns);
}
