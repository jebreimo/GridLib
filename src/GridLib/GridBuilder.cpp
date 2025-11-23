//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridBuilder.hpp"

#include <cmath>
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    Grid GridBuilder::build()
    {
        if (Xyz::get_length(model.row_axis()) == 0.0)
            GRIDLIB_THROW("Grid model has zero length row axis.");
        if (Xyz::get_length(model.column_axis()) == 0.0)
            GRIDLIB_THROW("Grid model has zero length col axis.");
        if (Xyz::get_length(model.vertical_axis()) == 0.0)
            GRIDLIB_THROW("Grid model has zero length vertical axis.");
        if (row_count == 0 || col_count == 0)
            GRIDLIB_THROW("Grid has zero size.");

        try
        {
            auto vals = std::move(values);

            auto grid = vals.empty()
                            ? Grid(Chorasmia::Array2D<float>({row_count, col_count}))
                            : Grid(Chorasmia::Array2D(std::move(vals),
                                                      {row_count, col_count}));
            grid.spatial_info() = model;
            return grid;
        }
        catch (Chorasmia::ChorasmiaException&)
        {
            std::throw_with_nested(GridLibException("Can't create grid"));
        }
    }
}
