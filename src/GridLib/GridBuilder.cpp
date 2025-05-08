//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-08.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridBuilder.hpp"

#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    namespace
    {
        void replace_nans(std::vector<float>& values, float value)
        {
            for (auto& v : values)
            {
                if (std::isnan(v))
                    v = value;
            }
        }
    }

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
            auto elevations = std::move(this->elevations);
            if (model.unknown_elevation && !isnan(*model.unknown_elevation))
                replace_nans(elevations, *model.unknown_elevation);

            auto grid = elevations.empty()
                            ? Grid(Chorasmia::Array2D<float>(row_count, col_count))
                            : Grid(Chorasmia::Array2D(std::move(elevations),
                                                      row_count, col_count));
            grid.set_model_tie_point(model_tie_point);
            grid.model() = model;
            grid.set_spatial_tie_points(std::move(spatial_tie_points));
            return grid;
        }
        catch (Chorasmia::ChorasmiaException&)
        {
            std::throw_with_nested(GridLibException("Can't create grid"));
        }
    }
}
