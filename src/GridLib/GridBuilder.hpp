//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "GridLib/Grid.hpp"

namespace GridLib
{
    struct GridBuilder
    {
        std::vector<float> values;
        size_t row_count = 0;
        size_t col_count = 0;
        Xyz::Vector2D model_tie_point;
        SpatialInfo model;
        std::vector<SpatialTiePoint> spatial_tie_points;

        Grid build();
    };
}
